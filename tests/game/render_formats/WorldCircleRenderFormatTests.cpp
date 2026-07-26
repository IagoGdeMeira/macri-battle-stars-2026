#include "game/render_formats/WorldCircleRenderFormat.h"

#include "StubRenderer.h"

#include "domain/components/CircleShapeComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class WorldCircleRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    WorldCircleRenderFormat format;
    RenderContext context;

    WorldCircleRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(10.f, 5.f);
        this->camera.setZoom(2.f);
    }
};

TEST_CASE_METHOD(WorldCircleRenderFormatFixture, "WorldCircleRenderFormat submits base and visual effect commands",
    "[unit][world_circle_render_format]"
) {
    const Entity entity = this->world.entities().create();

    Circle circle{Position{0.f, 0.f}, 5.f};

    auto& comp = this->world.components();
    comp.add<CircleShapeComponent>(entity, CircleShapeComponent{circle, Color{10, 20, 30, 255}, false, 0});
    comp.add<TransformComponent>(entity, TransformComponent{Position{30.f, 20.f}, Position{-2.f, 3.f}, 0.f});

    VisualEffectsComponent fx;
    fx.circleEffects.push_back([](DrawCircleBatch& batch, DrawCircleCommand& cmd) {
        DrawCircleCommand aura = cmd;
        aura.circle.radius += 2.f;
        aura.filled = true;
        batch.add(aura);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.size() == 2);

    const auto& effectCmd = this->renderer.circleCalls[0];
    REQUIRE(effectCmd.circle.radius == Catch::Approx(32.f));
    REQUIRE(effectCmd.filled == true);

    const auto& baseCmd = this->renderer.circleCalls[1];
    REQUIRE(baseCmd.circle.position.x == Catch::Approx(440.f));
    REQUIRE(baseCmd.circle.position.y == Catch::Approx(330.f));
    REQUIRE(baseCmd.circle.radius == Catch::Approx(30.f));
    REQUIRE(baseCmd.color == Color { 10, 20, 30, 255 });
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.order == 0);
}

TEST_CASE_METHOD(WorldCircleRenderFormatFixture, "WorldCircleRenderFormat filters non-circle and null shapes",
    "[unit][world_circle_render_format]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    
    const Entity nullShapeEntity = entities.create();
    comp.add<CircleShapeComponent>(nullShapeEntity, CircleShapeComponent{});
    comp.add<TransformComponent>(nullShapeEntity, TransformComponent{Position{0.f, 0.f}, Position{1.f, 1.f}, 0.f});

    const Entity rectangleEntity = entities.create();
    Rectangle rect{Position{0.f, 0.f}, Dimension2D{8.f, 6.f}};
    comp.add<RectangleShapeComponent>(rectangleEntity, RectangleShapeComponent{rect, Color::WHITE(), false, 0});
    comp.add<TransformComponent>(rectangleEntity, TransformComponent{Position{2.f, 3.f}, Position{1.f, 1.f}, 0.f});

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.empty());
}
