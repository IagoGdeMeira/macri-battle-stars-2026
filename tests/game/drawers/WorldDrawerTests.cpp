#include "game/drawers/WorldDrawer/WorldDrawer.h"

#include "StubRenderer.h"
#include "StubTexture.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/ShapeRenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Color/Color.h"

#include "engine/events/WindowResizedEvent.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/ThreadPool/ThreadPool.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include "game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <memory>
#include <vector>

class WorldDrawerFixture
{
public:
    GameSettings makeSettings()
    {
        GameSettings set;
        set.screen.size = { 1920.f, 1080.f };
        return set;
    }

    WorldDrawerFixture() :
        threadPool(1),
        textureFactory(),
        textureLoader(this->textureFactory),
        resourceManager(this->threadPool),
        settings(this->makeSettings()),
        drawer(WorldDrawer::Config{
            this->bus,
            this->renderer,
            this->camera,
            this->settings,
            this->resourceManager,
            this->textureLoader
        }),
        context{this->world, this->bus}
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<ShapeRenderComponent>();
        comp.registerComponent<AnimationControllerComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<VisualEffectsComponent>();
    }

protected:
    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    GameSettings settings;
    WorldDrawer drawer;
    RenderContext context;

    ThreadPool threadPool;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    ResourceManager resourceManager;
};

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer configures viewports on draw", "[unit][world_drawer]")
{
    this->drawer.draw(this->context);
    REQUIRE(this->renderer.calls.setViewport == 1);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer updates viewport on window resize", "[unit][world_drawer]")
{
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.setViewport == 1);
    REQUIRE(this->renderer.viewportHistory.size() == 1);
    REQUIRE(this->renderer.viewportHistory[0].x == 240);
    REQUIRE(this->renderer.viewportHistory[0].y == 0);
    REQUIRE(this->renderer.viewportHistory[0].width == 1440);
    REQUIRE(this->renderer.viewportHistory[0].height == 1080);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards resized world viewport to the world render formats",
    "[unit][world_drawer]"
) {
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });

    auto& comp = this->world.components();

    const auto spriteEntity = this->world.entities().create();
    comp.add<TransformComponent>(spriteEntity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(spriteEntity, SpriteComponent{ "dummy.png", 16, 8 });
    comp.add<RenderComponent>(spriteEntity, RenderComponent{ 0 });

    const auto rectangleEntity = this->world.entities().create();
    auto rectangleShape = std::make_unique<RectangleDef>();
    rectangleShape->width = 12.f;
    rectangleShape->height = 8.f;
    comp.add<TransformComponent>(rectangleEntity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<ShapeRenderComponent>(rectangleEntity, ShapeRenderComponent{std::move(rectangleShape), Color{1, 2, 3, 4}, true});

    const auto circleEntity = this->world.entities().create();
    auto circleShape = std::make_unique<CircleDef>();
    circleShape->radius = 7.f;
    comp.add<TransformComponent>(circleEntity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<ShapeRenderComponent>(circleEntity, ShapeRenderComponent{std::move(circleShape), Color{9, 8, 7, 6}, false});

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportHistory.size() == 1);
    REQUIRE(this->renderer.viewportHistory[0].x == 240);
    REQUIRE(this->renderer.viewportHistory[0].y == 0);
    REQUIRE(this->renderer.viewportHistory[0].width == 1440);
    REQUIRE(this->renderer.viewportHistory[0].height == 1080);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.x - 714.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.y - 548.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.width - 32.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.height - 24.f) < 0.001f);

    REQUIRE(this->renderer.rectangleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.x - 718.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.y - 548.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.width - 24.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.height - 24.f) < 0.001f);

    REQUIRE(this->renderer.circleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.x - 730.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.y - 560.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.radius - 21.f) < 0.001f);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws sprite using transformed world coordinates",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", 16, 8 });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.f, 0.f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 1);
    REQUIRE(this->renderer.lastDraw.x == 719);
    REQUIRE(this->renderer.lastDraw.y == 558);
    REQUIRE(this->renderer.lastDraw.width == 32);
    REQUIRE(this->renderer.lastDraw.height == 24);
    REQUIRE(this->renderer.lastDraw.rotation == 0.f);
    REQUIRE(this->renderer.lastDraw.flipX == false);
    REQUIRE(this->renderer.lastDraw.flipY == false);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards rotation and flip flags to renderer", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, -2.f, -3.f, 37.5f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", 16, 8 });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.f, 0.f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 1);
    REQUIRE(this->renderer.lastDraw.width == 32);
    REQUIRE(this->renderer.lastDraw.height == 24);
    REQUIRE(this->renderer.lastDraw.rotation == 37.5f);
    REQUIRE(this->renderer.lastDraw.flipX == true);
    REQUIRE(this->renderer.lastDraw.flipY == true);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards sprite source rect to renderer", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", 16, 8, 4, 6, 8, 10, true });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 1);
    REQUIRE(this->renderer.lastDraw.useSourceRect == true);
    REQUIRE(this->renderer.lastDraw.srcX == 4);
    REQUIRE(this->renderer.lastDraw.srcY == 6);
    REQUIRE(this->renderer.lastDraw.srcWidth == 8);
    REQUIRE(this->renderer.lastDraw.srcHeight == 10);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer skips sprites without textures", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ nullptr, 16, 8 });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 0);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws filled rectangle shapes", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    auto shape = std::make_unique<RectangleDef>();
    shape->width = 12.f;
    shape->height = 8.f;

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f });
    comp.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{1, 2, 3, 4}, true });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawRectFilled == 1);
    REQUIRE(this->renderer.calls.drawRectOutline == 0);
    REQUIRE(this->renderer.lastRect.position.x == 718.f);
    REQUIRE(this->renderer.lastRect.position.y == 548.f);
    REQUIRE(this->renderer.lastRect.size.width == 24.f);
    REQUIRE(this->renderer.lastRect.size.height == 24.f);
    REQUIRE(this->renderer.lastColor.r == 1);
    REQUIRE(this->renderer.lastColor.g == 2);
    REQUIRE(this->renderer.lastColor.b == 3);
    REQUIRE(this->renderer.lastColor.a == 4);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws outlined circle shapes", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    auto shape = std::make_unique<CircleDef>();
    shape->radius = 7.f;

    this->camera.setZoom(2.f);
    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, -3.f, 0.f});
    comp.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{9, 8, 7, 6}, false });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawCircleOutline == 1);
    REQUIRE(this->renderer.calls.drawCircleFilled == 0);
    REQUIRE(this->renderer.lastCircle.position.x == 740.f);
    REQUIRE(this->renderer.lastCircle.position.y == 580.f);
    REQUIRE(this->renderer.lastCircle.radius == 42.f);
    REQUIRE(this->renderer.lastColor.r == 9);
    REQUIRE(this->renderer.lastColor.g == 8);
    REQUIRE(this->renderer.lastColor.b == 7);
    REQUIRE(this->renderer.lastColor.a == 6);
}
