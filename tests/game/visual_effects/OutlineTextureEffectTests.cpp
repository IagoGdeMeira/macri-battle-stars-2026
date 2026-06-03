#include "../../../src/game/visual_effects/OutlineTextureEffect.h"

#include "../../../src/engine/include/DrawBatch/DrawTextureBatch.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

class OutlineTextureEffectFixture
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

TEST_CASE_METHOD(OutlineTextureEffectFixture, "OutlineTextureEffect adds 8 outline texture commands",
    "[unit][outline_texture_effect]"
) {
    OutlineComponent config;
    config.enabled = true;
    config.thickness = 2.0f;
    config.color = Color { 1, 2, 3, 4 };

    DrawTextureCommand base;
    base.dest.position = Position { 10.0f, 20.0f };
    base.tint = Color::WHITE();

    OutlineTextureEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.textureCalls.size() == 8);

    for (const auto& cmd : this->renderer.textureCalls)
    {
        REQUIRE(cmd.tint == config.color);
        const bool isBasePosition =
            (cmd.dest.position.x == 10.0f) &&
            (cmd.dest.position.y == 20.0f);
        REQUIRE_FALSE(isBasePosition);
    }
}
