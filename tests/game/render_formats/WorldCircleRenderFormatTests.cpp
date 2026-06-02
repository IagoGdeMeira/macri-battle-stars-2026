#include "../../src/game/render_formats/WorldCircleRenderFormat.h"

#include "../../src/domain/components/ShapeRenderComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VisualEffectsComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/RenderContext/RenderContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

class WorldCircleRenderFormatFixture
{
public:
    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}

        void drawCircle(const DrawCircleCommand& cmd) override
        { this->circleCalls.push_back(cmd); }

        void setViewport(const Viewport&) override {}

        std::vector<DrawCircleCommand> circleCalls;
    };

    WorldCircleRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<ShapeRenderComponent>();
        components.registerComponent<TransformComponent>();
        components.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(10.0f, 5.0f);
        this->camera.setZoom(2.0f);
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
    circle->radius = 5.0f;

    this->world.components().add<ShapeRenderComponent>(entity,
        ShapeRenderComponent { std::move(circle), Color { 10, 20, 30, 255 }, false });
    this->world.components().add<TransformComponent>(entity,
        TransformComponent { Position { 30.0f, 20.0f }, Position { -2.0f, 3.0f }, 0.0f });

    VisualEffectsComponent fx;
    fx.circleEffects.push_back([](DrawCircleBatch& batch, DrawCircleCommand& cmd) {
        DrawCircleCommand aura = cmd;
        aura.circle.radius += 2.0f;
        aura.filled = true;
        batch.add(aura);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.size() == 2);

    const auto& effectCmd = this->renderer.circleCalls[0];
    REQUIRE(effectCmd.circle.radius == Catch::Approx(32.0f));
    REQUIRE(effectCmd.filled == true);

    const auto& baseCmd = this->renderer.circleCalls[1];
    REQUIRE(baseCmd.circle.position.x == Catch::Approx(540.0f));
    REQUIRE(baseCmd.circle.position.y == Catch::Approx(330.0f));
    REQUIRE(baseCmd.circle.radius == Catch::Approx(30.0f));
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
        TransformComponent { Position { 0.0f, 0.0f }, Position { 1.0f, 1.0f }, 0.0f });

    const Entity rectangleEntity = this->world.entities().create();
    auto rect = std::make_unique<RectangleDef>();
    rect->width = 8.0f;
    rect->height = 6.0f;
    this->world.components().add<ShapeRenderComponent>(rectangleEntity,
        ShapeRenderComponent { std::move(rect), Color::WHITE(), false });
    this->world.components().add<TransformComponent>(rectangleEntity,
        TransformComponent { Position { 2.0f, 3.0f }, Position { 1.0f, 1.0f }, 0.0f });

    this->format.render(this->context);

    REQUIRE(this->renderer.circleCalls.empty());
}
