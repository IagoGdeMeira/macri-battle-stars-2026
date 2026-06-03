#include "../../../src/game/visual_effects/OutlineFontEffect.h"

#include "../../../src/engine/include/DrawBatch/DrawFontBatch.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

class OutlineFontEffectFixture
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

TEST_CASE_METHOD(OutlineFontEffectFixture, "OutlineFontEffect adds 8 outline font commands",
    "[unit][outline_font_effect]"
) {
    OutlineComponent config;
    config.enabled = true;
    config.thickness = 1.5f;
    config.color = Color { 101, 102, 103, 104 };

    DrawFontCommand base;
    base.dest.position = Position { 30.0f, 40.0f };
    base.color = Color::WHITE();

    OutlineFontEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.fontCalls.size() == 8);

    for (const auto& cmd : this->renderer.fontCalls)
    {
        REQUIRE(cmd.color == config.color);
        const bool isBasePosition =
            (cmd.dest.position.x == 30.0f) &&
            (cmd.dest.position.y == 40.0f);
        REQUIRE_FALSE(isBasePosition);
    }
}
