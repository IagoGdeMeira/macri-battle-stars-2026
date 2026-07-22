#include "game/visual_effects/ShadowFontEffect.h"

#include "StubRenderer.h"

#include "engine/draw_batches/DrawFontBatch.h"
#include "engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class TestDrawFontBatch : public DrawFontBatch
{
public:
    void submit(Renderer& renderer) override { DrawFontBatch::submit(renderer); }
};

class ShadowFontEffectFixture
{
public:
    StubRenderer renderer;
    TestDrawFontBatch batch;
};

TEST_CASE_METHOD(ShadowFontEffectFixture, "ShadowFontEffect adds offset font command",
    "[unit][shadow_font_effect]"
) {
    ShadowComponent config;
    config.enabled = true;
    config.offset = Position{-5.f, 6.f};
    config.color = Color{77, 66, 55, 44};

    DrawFontCommand base;
    base.dest.position = Position{30.f, 40.f};
    base.color = Color::WHITE();

    ShadowFontEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.fontCalls.size() == 1);
    REQUIRE(this->renderer.fontCalls[0].color == config.color);
    REQUIRE(this->renderer.fontCalls[0].dest.position.x == Catch::Approx(25.f));
    REQUIRE(this->renderer.fontCalls[0].dest.position.y == Catch::Approx(46.f));
}
