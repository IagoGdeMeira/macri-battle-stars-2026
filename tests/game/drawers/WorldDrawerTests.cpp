#include "game/drawers/WorldDrawer/WorldDrawer.h"

#include "StubRenderer.h"
#include "StubTexture.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Color/Color.h"

#include "engine/events/WindowResizedEvent.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/ThreadPool/ThreadPool.h"
#include "engine/value_objects/Camera2D/Camera2D.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <memory>
#include <vector>

class WorldDrawerFixture
{
public:
    WorldDrawerFixture() :
        threadPool(1),
        settings(this->makeSettings()),
        textureFactory(),
        textureLoader(this->textureFactory),
        resourceManager(this->threadPool),
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
        this->resourceManager.clearCache();
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<AnimationControllerComponent>();
        comp.registerComponent<CircleEffectsComponent>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleEffectsComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<TextureEffectsComponent>();
        comp.registerComponent<TransformComponent>();
    }

    GameSettings makeSettings()
    {
        GameSettings set;
        set.screen.size = {1920.f, 1080.f};
        return set;
    }

protected:
    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;

    ThreadPool threadPool;
    GameSettings settings;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    ResourceManager resourceManager;
    WorldDrawer drawer;
    RenderContext context;
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
    comp.add<SpriteComponent>(spriteEntity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 8} });
    comp.add<RenderComponent>(spriteEntity, RenderComponent{ 0 });

    const auto rectangleEntity = this->world.entities().create();
    Rectangle rect{Position{0.f, 0.f}, Dimension2D{12.f, 8.f}};
    comp.add<TransformComponent>(rectangleEntity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<RectangleShapeComponent>(rectangleEntity, RectangleShapeComponent{rect, Color{1, 2, 3, 4}, true, 0});

    const auto circleEntity = this->world.entities().create();
    Circle circle{Position{0.f, 0.f}, 7.f};
    comp.add<TransformComponent>(circleEntity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<CircleShapeComponent>(circleEntity, CircleShapeComponent{circle, Color{9, 8, 7, 6}, false, 0});

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportHistory.size() == 1);
    REQUIRE(this->renderer.viewportHistory[0].x == 240);
    REQUIRE(this->renderer.viewportHistory[0].y == 0);
    REQUIRE(this->renderer.viewportHistory[0].width == 1440);
    REQUIRE(this->renderer.viewportHistory[0].height == 1080);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.x - 394.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.y - 308.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.width - 32.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.height - 24.f) < 0.001f);

    REQUIRE(this->renderer.rectangleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.x - 398.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.y - 308.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.width - 24.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.height - 24.f) < 0.001f);

    REQUIRE(this->renderer.circleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.x - 410.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.y - 320.f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.radius - 21.f) < 0.001f);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws sprite using transformed world coordinates",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 8} });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.f, 0.f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 1);
    REQUIRE(this->renderer.lastDraw.x == 391);
    REQUIRE(this->renderer.lastDraw.y == 312);
    REQUIRE(this->renderer.lastDraw.width == 48);
    REQUIRE(this->renderer.lastDraw.height == 36);
    REQUIRE(this->renderer.lastDraw.rotation == 0.f);
    REQUIRE(this->renderer.lastDraw.flipX == false);
    REQUIRE(this->renderer.lastDraw.flipY == false);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards rotation and flip flags to renderer", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, -2.f, -3.f, 37.5f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 8} });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.f, 0.f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 1);
    REQUIRE(this->renderer.lastDraw.width == 48);
    REQUIRE(this->renderer.lastDraw.height == 36);
    REQUIRE(this->renderer.lastDraw.rotation == 37.5f);
    REQUIRE(this->renderer.lastDraw.flipX == true);
    REQUIRE(this->renderer.lastDraw.flipY == true);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards sprite source rect to renderer", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{ "dummy.png", nullptr, Dimension2D{16, 8}, Rectangle{4, 6, 8, 10}, true });
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
    comp.add<SpriteComponent>(entity, SpriteComponent{ "", nullptr, Dimension2D{16, 8} });
    comp.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawTexture == 0);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws filled rectangle shapes", "[unit][world_drawer]")
{
    const auto entity = this->world.entities().create();
    auto& comp = this->world.components();

    Rectangle rect{Position{0.f, 0.f}, Dimension2D{12.f, 8.f}};
    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, 3.f, 0.f});
    comp.add<RectangleShapeComponent>(entity, RectangleShapeComponent{rect, Color{1, 2, 3, 4}, true, 0});

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawRectFilled == 1);
    REQUIRE(this->renderer.calls.drawRectOutline == 0);

    REQUIRE(this->renderer.lastRect.position.x == 398.f);
    REQUIRE(this->renderer.lastRect.position.y == 308.f);
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

    Circle circle{Position{0.f, 0.f}, 7.f};
    comp.add<TransformComponent>(entity, TransformComponent{10.f, 20.f, 2.f, -3.f, 0.f});
    comp.add<CircleShapeComponent>(entity, CircleShapeComponent{circle, Color{9, 8, 7, 6}, false, 0});

    this->camera.setZoom(2.f);
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.drawCircleOutline == 1);
    REQUIRE(this->renderer.calls.drawCircleFilled == 0);
    REQUIRE(this->renderer.lastCircle.position.x == 420.f);
    REQUIRE(this->renderer.lastCircle.position.y == 340.f);
    REQUIRE(this->renderer.lastCircle.radius == 42.f);
    REQUIRE(this->renderer.lastColor.r == 9);
    REQUIRE(this->renderer.lastColor.g == 8);
    REQUIRE(this->renderer.lastColor.b == 7);
    REQUIRE(this->renderer.lastColor.a == 6);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer applies global scale from camera zoom", "[unit][world_drawer]")
{
    this->camera.setZoom(1.5f);
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.setScale == 2);

    REQUIRE(this->renderer.scaleHistory.size() == 2);
    REQUIRE(this->renderer.scaleHistory[0].x == Catch::Approx(1.8f));
    REQUIRE(this->renderer.scaleHistory[0].y == Catch::Approx(1.8f));
    REQUIRE(this->renderer.scaleHistory[1].x == Catch::Approx(1.f));
    REQUIRE(this->renderer.scaleHistory[1].y == Catch::Approx(1.f));

    REQUIRE(this->renderer.lastScale.x == Catch::Approx(1.f));
    REQUIRE(this->renderer.lastScale.y == Catch::Approx(1.f));
}
