#include "game/visual_effects/OutlineFontEffect.h"

#include "StubRenderer.h"

#include "engine/include/Renderer/Renderer.h"
#include "engine/include/RenderQueue/RenderQueue.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

class OutlineFontEffectFixture
{
public:
    StubRenderer renderer;
    RenderQueue queue;
};

TEST_CASE_METHOD(OutlineFontEffectFixture, "OutlineFontEffect adds 8 outline font commands",
    "[unit][outline_font_effect]"
) {
    OutlineComponent config;
    config.enabled = true;
    config.thickness = 1.5f;
    config.color = Color {101, 102, 103, 104};

    DrawFontCommand base;
    base.dest.position = Position {30.f, 40.f};
    base.color = Color::WHITE();

    OutlineFontEffect effect(config);
    effect.apply(this->queue, base);
    this->queue.submit(this->renderer);

    REQUIRE(this->renderer.fontCalls.size() == 8);

    for (const auto& cmd : this->renderer.fontCalls)
    {
        REQUIRE(cmd.color == config.color);

        auto& pos = cmd.dest.position;
        const bool isBasePosition = (pos.x == 30.f) && (pos.y == 40.f);
        REQUIRE_FALSE(isBasePosition);
    }
}
