#include "../../src/game/visual_effects/ShadowFontEffect.h"

#include "../../src/engine/include/DrawBatch/DrawFontBatch.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

class ShadowFontEffectFixture
{
public:
    class TestDrawFontBatch : public DrawFontBatch
    {
    public:
        void submit(Renderer& renderer) override { DrawFontBatch::submit(renderer); }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand&) override {}

        void drawFont(const DrawFontCommand& cmd) override
        { this->fontCalls.push_back(cmd); }

        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawFontCommand> fontCalls;
    };

    StubRenderer renderer;
    TestDrawFontBatch batch;
};

TEST_CASE_METHOD(ShadowFontEffectFixture, "ShadowFontEffect adds offset font command",
    "[unit][shadow_font_effect]"
) {
    ShadowComponent config;
    config.enabled = true;
    config.offset = Position { -5.0f, 6.0f };
    config.color = Color { 77, 66, 55, 44 };

    DrawFontCommand base;
    base.dest.position = Position { 30.0f, 40.0f };
    base.color = Color::WHITE();

    ShadowFontEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.fontCalls.size() == 1);
    REQUIRE(this->renderer.fontCalls[0].color == config.color);
    REQUIRE(this->renderer.fontCalls[0].dest.position.x == Catch::Approx(25.0f));
    REQUIRE(this->renderer.fontCalls[0].dest.position.y == Catch::Approx(46.0f));
}
