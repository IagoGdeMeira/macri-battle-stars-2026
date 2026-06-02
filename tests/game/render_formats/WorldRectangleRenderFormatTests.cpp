#include "../../src/game/render_formats/WorldRectangleRenderFormat.h"

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

class WorldRectangleRenderFormatFixture
{
public:
    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}

        void drawRectangle(const DrawRectangleCommand& cmd) override
        { this->rectangleCalls.push_back(cmd); }

        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawRectangleCommand> rectangleCalls;
    };

    WorldRectangleRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<ShapeRenderComponent>();
        components.registerComponent<TransformComponent>();
        components.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(5.0f, 0.0f);
        this->camera.setZoom(2.0f);
        this->format.setViewport(Viewport { 0, 0, 800, 600 });
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

    auto rect = std::make_unique<RectangleDef>();
    rect->width = 10.0f;
    rect->height = 4.0f;

    this->world.components().add<ShapeRenderComponent>(entity,
        ShapeRenderComponent { std::move(rect), Color { 100, 150, 200, 255 }, true });
    this->world.components().add<TransformComponent>(entity,
        TransformComponent { Position { 20.0f, 10.0f }, Position { -2.0f, 3.0f }, 0.0f });

    VisualEffectsComponent fx;
    fx.rectangleEffects.push_back([](DrawRectangleBatch& batch, DrawRectangleCommand& cmd) {
        DrawRectangleCommand outline = cmd;
        outline.filled = false;
        outline.color = Color { 7, 8, 9, 255 };
        batch.add(outline);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.rectangleCalls.size() == 2);

    const auto& effectCmd = this->renderer.rectangleCalls[0];
    REQUIRE(effectCmd.filled == false);
    REQUIRE(effectCmd.color == Color { 7, 8, 9, 255 });

    const auto& baseCmd = this->renderer.rectangleCalls[1];
    REQUIRE(baseCmd.rect.position.x == Catch::Approx(410.0f));
    REQUIRE(baseCmd.rect.position.y == Catch::Approx(308.0f));
    REQUIRE(baseCmd.rect.size.width == Catch::Approx(40.0f));
    REQUIRE(baseCmd.rect.size.height == Catch::Approx(24.0f));
    REQUIRE(baseCmd.filled == true);
    REQUIRE(baseCmd.color == Color { 100, 150, 200, 255 });
    REQUIRE(baseCmd.order == 0);
}

TEST_CASE_METHOD(WorldRectangleRenderFormatFixture, "WorldRectangleRenderFormat filters non-rectangle and null shapes",
    "[unit][world_rectangle_render_format]"
) {
    const Entity nullShapeEntity = this->world.entities().create();
    this->world.components().add<ShapeRenderComponent>(nullShapeEntity, ShapeRenderComponent {});
    this->world.components().add<TransformComponent>(nullShapeEntity,
        TransformComponent { Position { 0.0f, 0.0f }, Position { 1.0f, 1.0f }, 0.0f });

    const Entity circleEntity = this->world.entities().create();
    auto circle = std::make_unique<CircleDef>();
    circle->radius = 5.0f;
    this->world.components().add<ShapeRenderComponent>(circleEntity,
        ShapeRenderComponent { std::move(circle), Color::WHITE(), false });
    this->world.components().add<TransformComponent>(circleEntity,
        TransformComponent { Position { 2.0f, 3.0f }, Position { 1.0f, 1.0f }, 0.0f });

    this->format.render(this->context);

    REQUIRE(this->renderer.rectangleCalls.empty());
}
