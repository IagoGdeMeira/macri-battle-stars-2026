#include "game/visual_effects/ShadowTextureEffect.h"

#include "StubRenderer.h"

#include "engine/include/DrawBatch/DrawTextureBatch.h"
#include "engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class ShadowTextureEffectFixture
{
public:
    StubRenderer renderer;
    DrawTextureBatch batch;
};

TEST_CASE_METHOD(ShadowTextureEffectFixture, "ShadowTextureEffect adds offset texture command",
    "[unit][shadow_texture_effect]"
) {
    ShadowComponent config;
    config.enabled = true;
    config.offset = Position{3.f, -4.f};
    config.color = Color{9, 8, 7, 6};

    DrawTextureCommand base;
    base.dest.position = Position{10.f, 20.f};
    base.tint = Color::WHITE();

    ShadowTextureEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(this->renderer.textureCalls[0].tint == config.color);
    REQUIRE(this->renderer.textureCalls[0].dest.position.x == Catch::Approx(13.f));
    REQUIRE(this->renderer.textureCalls[0].dest.position.y == Catch::Approx(16.f));
}
