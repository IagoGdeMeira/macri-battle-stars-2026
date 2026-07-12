#include "../../../src/game/render_formats/UICircleRenderFormat.h"

#include "../../stubs/StubRenderer.h"

#include "../../../src/domain/components/RenderComponent.h"
#include "../../../src/domain/components/UITransform.h"
#include "../../../src/domain/components/VisualEffectsComponent.h"
#include "../../../src/domain/include/World/World.h"

#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/RenderContext/RenderContext.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

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
        comp.registerComponent<UITransform>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<VisualEffectsComponent>();
    }
};

TEST_CASE_METHOD(UICircleRenderFormatFixture, "UICircleRenderFormat submits base and visual effect commands",
    "[unit][circle_render_format]"
) {
    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle{Position{10.f, 20.f}, Dimension2D{30.f, 20.f}};

    auto& comp = this->world.components();
    comp.add<UITransform>(entity, transform);
    comp.add<RenderComponent>(entity, RenderComponent { 8, 2 });

    VisualEffectsComponent fx;
    fx.circleEffects.push_back([](DrawCircleBatch& batch, DrawCircleCommand& cmd) {
        DrawCircleCommand halo = cmd;
        halo.filled = true;
        halo.color = Color {7, 6, 5, 4};
        halo.circle.radius += 1.f;
        batch.add(halo);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.size() == 2);

    const auto& effectCmd = this->renderer.circleCalls[0];
    REQUIRE(effectCmd.filled == true);
    REQUIRE(effectCmd.color == Color {7, 6, 5, 4});
    REQUIRE(effectCmd.circle.radius == Catch::Approx(11.f));

    const auto& baseCmd = this->renderer.circleCalls[1];
    REQUIRE(baseCmd.circle.position.x == Catch::Approx(25.f));
    REQUIRE(baseCmd.circle.position.y == Catch::Approx(30.f));
    REQUIRE(baseCmd.circle.radius == Catch::Approx(10.f));
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.color == Color::WHITE());
    REQUIRE(baseCmd.layer == 8);
    REQUIRE(baseCmd.zIndex == 2);
    REQUIRE(baseCmd.order == 0);
}
