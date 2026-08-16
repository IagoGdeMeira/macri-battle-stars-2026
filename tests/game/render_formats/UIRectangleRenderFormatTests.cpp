#include "game/render_formats/UIRectangleRenderFormat.h"

#include "StubRenderer.h"

#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

class UIRectangleRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    UIRectangleRenderFormat format;
    RenderContext context;

    UIRectangleRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<RectangleEffectsComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
    }
};

TEST_CASE_METHOD(UIRectangleRenderFormatFixture, "UIRectangleRenderFormat submits base and visual effect commands",
    "[unit][rectangle_render_format]"
) {
    Entity entity = this->world.entities().create();

    TransformComponent tc;
    tc.position = {15.f, 25.f};

    UIRectComponent urc;
    urc.size = {40.f, 12.f};

    auto& comp = this->world.components();
    comp.add<TransformComponent>(entity, tc);
    comp.add<UIRectComponent>(entity, urc);
    comp.add<RenderComponent>(entity, RenderComponent { 3, 9 });

    RectangleEffectsComponent fx;
    fx.effects.push_back([](void* q, void* c) {
        auto& queue = *static_cast<RenderQueue*>(q);
        auto& cmd = *static_cast<DrawRectangleCommand*>(c);
        auto glow = std::make_unique<DrawRectangleCommand>(cmd);
        glow->filled = true;
        glow->color = Color {9, 9, 9, 255};
        queue.add(std::move(glow));
    });
    comp.add<RectangleEffectsComponent>(entity, fx);

    RenderQueue queue;
    this->format.render(this->context, queue);
    queue.submit(this->renderer);

    REQUIRE(this->renderer.rectangleCalls.size() == 2);

    const auto& baseCmd = this->renderer.rectangleCalls[0];
    REQUIRE(baseCmd.rect.position.x == Catch::Approx(15.f));
    REQUIRE(baseCmd.rect.position.y == Catch::Approx(25.f));
    REQUIRE(baseCmd.rect.size.width == Catch::Approx(40.f));
    REQUIRE(baseCmd.rect.size.height == Catch::Approx(12.f));
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.color == Color::WHITE());
    REQUIRE(baseCmd.layer == 3);
    REQUIRE(baseCmd.zIndex == 9);
    REQUIRE(baseCmd.order == 0);

    const auto& effectCmd = this->renderer.rectangleCalls[1];
    REQUIRE(effectCmd.filled == true);
    REQUIRE(effectCmd.color == Color {9, 9, 9, 255});
}
