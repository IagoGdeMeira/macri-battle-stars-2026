#include "../src/game/include/WorldDrawer/WorldDrawer.h"

#include "../src/domain/components/AnimationControllerComponent.h"
#include "../src/domain/components/OrientationComponent.h"
#include "../src/domain/components/ParallaxComponent.h"
#include "../src/domain/components/RenderComponent.h"
#include "../src/domain/components/ShapeRenderComponent.h"
#include "../src/domain/components/SpriteComponent.h"
#include "../src/domain/components/TransformComponent.h"
#include "../src/domain/components/VisualEffectsComponent.h"
#include "../src/domain/include/Color/Color.h"
#include "../src/domain/include/World/World.h"

#include "../src/engine/events/WindowResizedEvent.h"
#include "../src/engine/include/EventBus/EventBus.h"
#include "../src/engine/include/GameSettings/GameSettings.h"
#include "../src/engine/include/RenderContext/RenderContext.h"
#include "../src/engine/include/Renderer/Renderer.h"

#include "../src/game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <memory>
#include <vector>

class WorldDrawerFixture
{
public:
    static GameSettings makeSettings()
    {
        GameSettings settings;
        settings.screen.size = { 1920.0f, 1080.0f };
        return settings;
    }

    WorldDrawerFixture() : settings(WorldDrawerFixture::makeSettings()), drawer(bus, renderer, camera, settings), context
        {world, bus}
    {
        auto& components = this->world.components();

        components.registerComponent<TransformComponent>();
        components.registerComponent<SpriteComponent>();
        components.registerComponent<RenderComponent>();
        components.registerComponent<ParallaxComponent>();
        components.registerComponent<ShapeRenderComponent>();
        components.registerComponent<AnimationControllerComponent>();
        components.registerComponent<OrientationComponent>();
        components.registerComponent<VisualEffectsComponent>();
    }

protected:
    struct StubTexture : Texture
    {
        int getWidth() const override { return 16; }
        int getHeight() const override { return 8; }
    };

    struct StubRenderer : Renderer
    {
        int clearCalls = 0;
        int presentCalls = 0;
        int drawTextureCalls = 0;
        int drawRectOutlineCalls = 0;
        int drawRectFilledCalls = 0;
        int drawCircleOutlineCalls = 0;
        int drawCircleFilledCalls = 0;
        int viewportCalls = 0;
        std::string lastTextureId;
        int lastDrawX = 0;
        int lastDrawY = 0;
        int lastDrawWidth = 0;
        int lastDrawHeight = 0;
        float lastDrawRotation = 0.0f;
        bool lastDrawFlipX = false;
        bool lastDrawFlipY = false;
        int lastSrcX = 0;
        int lastSrcY = 0;
        int lastSrcWidth = 0;
        int lastSrcHeight = 0;
        bool lastUseSourceRect = false;
        Rectangle lastRect;
        Circle lastCircle;
        Color lastColor;
        int viewportX = 0;
        int viewportY = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;
        std::vector<Viewport> viewportHistory;
        std::vector<DrawTextureCommand> textureCalls;
        std::vector<DrawRectangleCommand> rectangleCalls;
        std::vector<DrawCircleCommand> circleCalls;

        void clear() override { this->clearCalls++; }
        void present() override { this->presentCalls++; }

        void drawTexture(const DrawTextureCommand& cmd) override
        {
            this->drawTextureCalls++;
            this->textureCalls.push_back(cmd);
            this->lastDrawX = static_cast<int>(cmd.dest.position.x);
            this->lastDrawY = static_cast<int>(cmd.dest.position.y);
            this->lastDrawWidth = static_cast<int>(cmd.dest.size.width);
            this->lastDrawHeight = static_cast<int>(cmd.dest.size.height);
            this->lastDrawRotation = cmd.rotation;
            this->lastDrawFlipX = cmd.flipX;
            this->lastDrawFlipY = cmd.flipY;
            this->lastSrcX = static_cast<int>(cmd.source.position.x);
            this->lastSrcY = static_cast<int>(cmd.source.position.y);
            this->lastSrcWidth = static_cast<int>(cmd.source.size.width);
            this->lastSrcHeight = static_cast<int>(cmd.source.size.height);
            this->lastUseSourceRect = cmd.useSourceRect;
        }

        void drawFont(const DrawFontCommand& cmd) override { (void)cmd; }

        void drawRectangle(const DrawRectangleCommand& cmd) override
        {
            if (cmd.filled) this->drawRectFilledCalls++;
            else this->drawRectOutlineCalls++;
            this->rectangleCalls.push_back(cmd);
            this->lastRect = cmd.rect;
            this->lastColor = cmd.color;
        }

        void drawCircle(const DrawCircleCommand& cmd) override
        {
            if (cmd.filled) this->drawCircleFilledCalls++;
            else this->drawCircleOutlineCalls++;
            this->circleCalls.push_back(cmd);
            this->lastCircle = cmd.circle;
            this->lastColor = cmd.color;
        }

        void setViewport(const Viewport& viewport) override
        {
            this->viewportCalls++;
            this->viewportX = viewport.x;
            this->viewportY = viewport.y;
            this->viewportWidth = viewport.width;
            this->viewportHeight = viewport.height;
            this->viewportHistory.push_back(viewport);
        }
    };

    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    GameSettings settings;
    WorldDrawer drawer;
    RenderContext context;
};

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer configures viewports on draw",
    "[unit][world_drawer]"
) {
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 1);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer updates viewport on window resize",
    "[unit][world_drawer]"
) {
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 1);
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

    auto& components = this->world.components();

    const auto spriteEntity = this->world.entities().create();
    components.add<TransformComponent>(spriteEntity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(spriteEntity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8 });
    components.add<RenderComponent>(spriteEntity, RenderComponent{ 0 });

    const auto rectangleEntity = this->world.entities().create();
    auto rectangleShape = std::make_unique<RectangleDef>();
    rectangleShape->width = 12.0f;
    rectangleShape->height = 8.0f;
    components.add<TransformComponent>(rectangleEntity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<ShapeRenderComponent>(rectangleEntity, ShapeRenderComponent{
        std::move(rectangleShape), Color{ 1, 2, 3, 4 }, true });

    const auto circleEntity = this->world.entities().create();
    auto circleShape = std::make_unique<CircleDef>();
    circleShape->radius = 7.0f;
    components.add<TransformComponent>(circleEntity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<ShapeRenderComponent>(circleEntity, ShapeRenderComponent{
        std::move(circleShape), Color{ 9, 8, 7, 6 }, false });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportHistory.size() == 1);
    REQUIRE(this->renderer.viewportHistory[0].x == 240);
    REQUIRE(this->renderer.viewportHistory[0].y == 0);
    REQUIRE(this->renderer.viewportHistory[0].width == 1440);
    REQUIRE(this->renderer.viewportHistory[0].height == 1080);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.x - 730.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.position.y - 560.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.width - 32.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.textureCalls[0].dest.size.height - 24.0f) < 0.001f);

    REQUIRE(this->renderer.rectangleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.x - 718.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.position.y - 548.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.width - 24.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.rectangleCalls[0].rect.size.height - 24.0f) < 0.001f);

    REQUIRE(this->renderer.circleCalls.size() == 1);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.x - 730.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.position.y - 560.0f) < 0.001f);
    REQUIRE(std::abs(this->renderer.circleCalls[0].circle.radius - 21.0f) < 0.001f);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws sprite using transformed world coordinates",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastDrawX == 735);
    REQUIRE(this->renderer.lastDrawY == 570);
    REQUIRE(this->renderer.lastDrawWidth == 48);
    REQUIRE(this->renderer.lastDrawHeight == 36);
    REQUIRE(this->renderer.lastDrawRotation == 0.0f);
    REQUIRE(this->renderer.lastDrawFlipX == false);
    REQUIRE(this->renderer.lastDrawFlipY == false);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards rotation and flip flags to renderer",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, -2.0f, -3.0f, 37.5f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastDrawWidth == 48);
    REQUIRE(this->renderer.lastDrawHeight == 36);
    REQUIRE(this->renderer.lastDrawRotation == 37.5f);
    REQUIRE(this->renderer.lastDrawFlipX == true);
    REQUIRE(this->renderer.lastDrawFlipY == true);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer forwards sprite source rect to renderer",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8, 4, 6, 8, 10, true });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastUseSourceRect == true);
    REQUIRE(this->renderer.lastSrcX == 4);
    REQUIRE(this->renderer.lastSrcY == 6);
    REQUIRE(this->renderer.lastSrcWidth == 8);
    REQUIRE(this->renderer.lastSrcHeight == 10);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer skips sprites without textures",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ nullptr, 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 0);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws filled rectangle shapes",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    auto shape = std::make_unique<RectangleDef>();
    shape->width = 12.0f;
    shape->height = 8.0f;

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{ 1, 2, 3, 4 }, true });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawRectFilledCalls == 1);
    REQUIRE(this->renderer.drawRectOutlineCalls == 0);
    REQUIRE(this->renderer.lastRect.position.x == 718.0f);
    REQUIRE(this->renderer.lastRect.position.y == 548.0f);
    REQUIRE(this->renderer.lastRect.size.width == 24.0f);
    REQUIRE(this->renderer.lastRect.size.height == 24.0f);
    REQUIRE(this->renderer.lastColor.r == 1);
    REQUIRE(this->renderer.lastColor.g == 2);
    REQUIRE(this->renderer.lastColor.b == 3);
    REQUIRE(this->renderer.lastColor.a == 4);
}

TEST_CASE_METHOD(WorldDrawerFixture, "WorldDrawer draws outlined circle shapes",
    "[unit][world_drawer]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    auto shape = std::make_unique<CircleDef>();
    shape->radius = 7.0f;

    this->camera.setZoom(2.0f);
    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, -3.0f, 0.0f });
    components.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{ 9, 8, 7, 6 }, false });

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.drawCircleOutlineCalls == 1);
    REQUIRE(this->renderer.drawCircleFilledCalls == 0);
    REQUIRE(this->renderer.lastCircle.position.x == 740.0f);
    REQUIRE(this->renderer.lastCircle.position.y == 580.0f);
    REQUIRE(this->renderer.lastCircle.radius == 42.0f);
    REQUIRE(this->renderer.lastColor.r == 9);
    REQUIRE(this->renderer.lastColor.g == 8);
    REQUIRE(this->renderer.lastColor.b == 7);
    REQUIRE(this->renderer.lastColor.a == 6);
}
