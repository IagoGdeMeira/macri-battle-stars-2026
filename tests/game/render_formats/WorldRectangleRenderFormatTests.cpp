#include "game/render_formats/WorldRectangleRenderFormat.h"

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

class WorldRectangleRenderFormatFixture
{
public:
    WorldRectangleRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(5.f, 0.f);
        this->camera.setZoom(2.f);
        this->format.setViewport(Viewport{0, 0, 800, 600});
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    WorldRectangleRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(WorldRectangleRenderFormatFixture, "WorldRectangleRenderFormat submits base and visual effect commands",
    "[unit][world_rectangle_render_format]"
) {
    const Entity entity = this->world.entities().create();

    Rectangle rect{Position{0.f, 0.f}, Dimension2D{10.f, 4.f}};

    auto& comp = this->world.components();
    comp.add<RectangleShapeComponent>(entity, RectangleShapeComponent{rect, Color{100, 150, 200, 255}, true, 0});
    comp.add<TransformComponent>(entity, TransformComponent{Position{20.f, 10.f}, Position{-2.f, 3.f}, 0.f});

    VisualEffectsComponent fx;
    fx.rectangleEffects.push_back([](DrawRectangleBatch& batch, DrawRectangleCommand& cmd) {
        DrawRectangleCommand outline = cmd;
        outline.filled = false;
        outline.color = Color { 7, 8, 9, 255 };
        batch.add(outline);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.rectangleCalls.size() == 2);

    const auto& effectCmd = this->renderer.rectangleCalls[0];
    REQUIRE(effectCmd.filled == false);
    REQUIRE(effectCmd.color == Color { 7, 8, 9, 255 });

    const auto& baseCmd = this->renderer.rectangleCalls[1];
    REQUIRE(baseCmd.rect.position.x == Catch::Approx(410.f));
    REQUIRE(baseCmd.rect.position.y == Catch::Approx(308.f));
    REQUIRE(baseCmd.rect.size.width == Catch::Approx(40.f));
    REQUIRE(baseCmd.rect.size.height == Catch::Approx(24.f));
    REQUIRE(baseCmd.filled == true);
    REQUIRE(baseCmd.color == Color { 100, 150, 200, 255 });
    REQUIRE(baseCmd.order == 0);
}

TEST_CASE_METHOD(WorldRectangleRenderFormatFixture, "WorldRectangleRenderFormat filters non-rectangle and null shapes",
    "[unit][world_rectangle_render_format]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    const Entity nullShapeEntity = entities.create();
    comp.add<RectangleShapeComponent>(nullShapeEntity, RectangleShapeComponent{});
    comp.add<TransformComponent>(nullShapeEntity, TransformComponent{Position{0.f, 0.f}, Position{1.f, 1.f}, 0.f});

    const Entity circleEntity = entities.create();
    Circle circle{Position{0.f, 0.f}, 5.f};
    comp.add<CircleShapeComponent>(circleEntity, CircleShapeComponent{circle, Color::WHITE(), false, 0});
    comp.add<TransformComponent>(circleEntity, TransformComponent{Position{2.f, 3.f}, Position{1.f, 1.f}, 0.f});

    this->format.render(this->context);

    REQUIRE(this->renderer.rectangleCalls.empty());
}
