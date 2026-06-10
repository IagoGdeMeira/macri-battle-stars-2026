#include "../../../src/game/render_formats/WorldCircleRenderFormat.h"

#include "../../stubs/StubRenderer.h"

#include "../../../src/domain/components/ShapeRenderComponent.h"
#include "../../../src/domain/components/TransformComponent.h"
#include "../../../src/domain/components/VisualEffectsComponent.h"
#include "../../../src/domain/include/World/World.h"

#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/RenderContext/RenderContext.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class WorldCircleRenderFormatFixture
{
public:
    WorldCircleRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<ShapeRenderComponent>();
        components.registerComponent<TransformComponent>();
        components.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(10.f, 5.f);
        this->camera.setZoom(2.f);
        this->format.setViewport(Viewport { 0, 0, 1000, 600 });
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    WorldCircleRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(WorldCircleRenderFormatFixture, "WorldCircleRenderFormat submits base and visual effect commands",
    "[unit][world_circle_render_format]"
) {
    const Entity entity = this->world.entities().create();

    auto circle = std::make_unique<CircleDef>();
    circle->radius = 5.f;

    this->world.components().add<ShapeRenderComponent>(entity,
        ShapeRenderComponent { std::move(circle), Color { 10, 20, 30, 255 }, false });
    this->world.components().add<TransformComponent>(entity,
        TransformComponent{Position{30.f, 20.f}, Position{-2.f, 3.f}, 0.f});

    VisualEffectsComponent fx;
    fx.circleEffects.push_back([](DrawCircleBatch& batch, DrawCircleCommand& cmd) {
        DrawCircleCommand aura = cmd;
        aura.circle.radius += 2.f;
        aura.filled = true;
        batch.add(aura);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.size() == 2);

    const auto& effectCmd = this->renderer.circleCalls[0];
    REQUIRE(effectCmd.circle.radius == Catch::Approx(32.f));
    REQUIRE(effectCmd.filled == true);

    const auto& baseCmd = this->renderer.circleCalls[1];
    REQUIRE(baseCmd.circle.position.x == Catch::Approx(540.f));
    REQUIRE(baseCmd.circle.position.y == Catch::Approx(330.f));
    REQUIRE(baseCmd.circle.radius == Catch::Approx(30.f));
    REQUIRE(baseCmd.color == Color { 10, 20, 30, 255 });
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.order == 0);
}

TEST_CASE_METHOD(WorldCircleRenderFormatFixture, "WorldCircleRenderFormat filters non-circle and null shapes",
    "[unit][world_circle_render_format]"
) {
    const Entity nullShapeEntity = this->world.entities().create();
    this->world.components().add<ShapeRenderComponent>(nullShapeEntity, ShapeRenderComponent {});
    this->world.components().add<TransformComponent>(nullShapeEntity,
        TransformComponent{Position{0.f, 0.f}, Position{1.f, 1.f}, 0.f});

    const Entity rectangleEntity = this->world.entities().create();
    auto rect = std::make_unique<RectangleDef>();
    rect->width = 8.f;
    rect->height = 6.f;
    this->world.components().add<ShapeRenderComponent>(rectangleEntity,
        ShapeRenderComponent { std::move(rect), Color::WHITE(), false });
    this->world.components().add<TransformComponent>(rectangleEntity,
        TransformComponent{Position{2.f, 3.f}, Position{1.f, 1.f}, 0.f});

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.empty());
}
