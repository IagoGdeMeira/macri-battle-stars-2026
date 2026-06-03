#include "../../../src/game/visual_effects/ShadowTextureEffect.h"

#include "../../../src/engine/include/Renderer/Renderer.h"
#include "../../../src/engine/include/DrawBatch/DrawTextureBatch.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

class ShadowTextureEffectFixture
{
public:
    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand& cmd) override
        { this->textureCalls.push_back(cmd); }

        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawTextureCommand> textureCalls;
    };

    StubRenderer renderer;
    DrawTextureBatch batch;
};

TEST_CASE_METHOD(ShadowTextureEffectFixture, "ShadowTextureEffect adds offset texture command",
    "[unit][shadow_texture_effect]"
) {
    ShadowComponent config;
    config.enabled = true;
    config.offset = Position { 3.0f, -4.0f };
    config.color = Color { 9, 8, 7, 6 };

    DrawTextureCommand base;
    base.dest.position = Position { 10.0f, 20.0f };
    base.tint = Color::WHITE();

    ShadowTextureEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(this->renderer.textureCalls[0].tint == config.color);
    REQUIRE(this->renderer.textureCalls[0].dest.position.x == Catch::Approx(13.0f));
    REQUIRE(this->renderer.textureCalls[0].dest.position.y == Catch::Approx(16.0f));
}
