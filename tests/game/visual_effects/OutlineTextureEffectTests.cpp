#include "game/visual_effects/OutlineTextureEffect.h"

#include "StubRenderer.h"

#include "engine/include/DrawBatch/DrawTextureBatch.h"
#include "engine/include/Renderer/Renderer.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class OutlineTextureEffectFixture
{
public:
    StubRenderer renderer;
    DrawTextureBatch batch;
};

TEST_CASE_METHOD(OutlineTextureEffectFixture, "OutlineTextureEffect adds 8 outline texture commands",
    "[unit][outline_texture_effect]"
) {
    OutlineComponent config;
    config.enabled = true;
    config.thickness = 2.f;
    config.color = Color {1, 2, 3, 4};

    DrawTextureCommand base;
    base.dest.position = Position {10.f, 20.f};
    base.tint = Color::WHITE();

    OutlineTextureEffect effect(config);
    effect.apply(this->batch, base);
    this->batch.submit(this->renderer);

    REQUIRE(this->renderer.textureCalls.size() == 8);

    for (const auto& cmd : this->renderer.textureCalls)
    {
        REQUIRE(cmd.tint == config.color);

        auto& pos = cmd.dest.position;
        const bool isBasePosition = (pos.x == 10.f) && (pos.y == 20.f);
        REQUIRE_FALSE(isBasePosition);
    }
}
