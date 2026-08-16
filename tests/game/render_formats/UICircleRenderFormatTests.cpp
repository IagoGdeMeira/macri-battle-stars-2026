#include "game/render_formats/UICircleRenderFormat.h"

#include "StubRenderer.h"

#include "domain/components/CircleEffectsComponent.h"
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

class UICircleRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    UICircleRenderFormat format;
    RenderContext context;

    UICircleRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<CircleEffectsComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
    }
};

TEST_CASE_METHOD(UICircleRenderFormatFixture, "UICircleRenderFormat submits base and visual effect commands",
    "[unit][circle_render_format]"
) {
    Entity entity = this->world.entities().create();

    TransformComponent tc;
    tc.position = {10.f, 20.f};

    UIRectComponent urc;
    urc.size = {30.f, 20.f};

    auto& comp = this->world.components();
    comp.add<TransformComponent>(entity, tc);
    comp.add<UIRectComponent>(entity, urc);
    comp.add<RenderComponent>(entity, RenderComponent { 8, 2 });

    CircleEffectsComponent fx;
    fx.effects.push_back([](void* q, void* c) {
        auto& queue = *static_cast<RenderQueue*>(q);
        auto& cmd = *static_cast<DrawCircleCommand*>(c);
        auto halo = std::make_unique<DrawCircleCommand>(cmd);
        halo->filled = true;
        halo->color = Color {7, 6, 5, 4};
        halo->circle.radius += 1.f;
        queue.add(std::move(halo));
    });
    comp.add<CircleEffectsComponent>(entity, fx);

    RenderQueue queue;
    this->format.render(this->context, queue);
    queue.submit(this->renderer);

    REQUIRE(this->renderer.circleCalls.size() == 2);

    const auto& baseCmd = this->renderer.circleCalls[0];
    REQUIRE(baseCmd.circle.position.x == Catch::Approx(25.f));
    REQUIRE(baseCmd.circle.position.y == Catch::Approx(30.f));
    REQUIRE(baseCmd.circle.radius == Catch::Approx(10.f));
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.color == Color::WHITE());
    REQUIRE(baseCmd.layer == 8);
    REQUIRE(baseCmd.zIndex == 2);
    REQUIRE(baseCmd.order == 0);

    const auto& effectCmd = this->renderer.circleCalls[1];
    REQUIRE(effectCmd.filled == true);
    REQUIRE(effectCmd.color == Color {7, 6, 5, 4});
    REQUIRE(effectCmd.circle.radius == Catch::Approx(11.f));
}
