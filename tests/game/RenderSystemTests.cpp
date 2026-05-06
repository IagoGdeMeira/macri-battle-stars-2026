#include "../../src/game/include/RenderSystem/RenderSystem.h"

#include "../../src/domain/components/AnimationControllerComponent.h"
#include "../../src/domain/components/OrientationComponent.h"
#include "../../src/domain/components/ParallaxComponent.h"
#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/ShapeRenderComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/Color/Color.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/events/WindowResizedEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/RenderContext/RenderContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include "../../src/game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class RenderSystemFixture
{
public:
    RenderSystemFixture() : system(bus, renderer, camera), context{ world, bus }
    {
        auto& components = this->world.components();

        components.registerComponent<TransformComponent>();
        components.registerComponent<SpriteComponent>();
        components.registerComponent<RenderComponent>();
        components.registerComponent<ParallaxComponent>();
        components.registerComponent<ShapeRenderComponent>();
        components.registerComponent<AnimationControllerComponent>();
        components.registerComponent<OrientationComponent>();
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

        void clear() override { this->clearCalls++; }
        void present() override { this->presentCalls++; }

        void drawTexture(const Texture& texture, const Renderer::DrawTextureParams& params) override
        {
            (void)texture;
            this->drawTextureCalls++;
            this->lastDrawX = static_cast<int>(params.dest.position.x);
            this->lastDrawY = static_cast<int>(params.dest.position.y);
            this->lastDrawWidth = static_cast<int>(params.dest.width);
            this->lastDrawHeight = static_cast<int>(params.dest.height);
            this->lastDrawRotation = params.rotation;
            this->lastDrawFlipX = params.flipX;
            this->lastDrawFlipY = params.flipY;
            this->lastSrcX = static_cast<int>(params.source.position.x);
            this->lastSrcY = static_cast<int>(params.source.position.y);
            this->lastSrcWidth = static_cast<int>(params.source.width);
            this->lastSrcHeight = static_cast<int>(params.source.height);
            this->lastUseSourceRect = params.useSourceRect;
        }

        void drawRectOutline(const Rectangle& rect, const Color& color) override
        {
            this->drawRectOutlineCalls++;
            this->lastRect = rect;
            this->lastColor = color;
        }

        void drawRectFilled(const Rectangle& rect, const Color& color) override
        {
            this->drawRectFilledCalls++;
            this->lastRect = rect;
            this->lastColor = color;
        }

        void drawCircleOutline(const Circle& circle, const Color& color) override
        {
            this->drawCircleOutlineCalls++;
            this->lastCircle = circle;
            this->lastColor = color;
        }

        void drawCircleFilled(const Circle& circle, const Color& color) override
        {
            this->drawCircleFilledCalls++;
            this->lastCircle = circle;
            this->lastColor = color;
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
    RenderSystem system;
    RenderContext context;
};

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem configures viewports on draw",
    "[unit][render_system]"
) {
    this->system.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 2);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem updates viewport on window resize",
    "[unit][render_system]"
) {
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });

    this->system.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 2);
    REQUIRE(this->renderer.viewportHistory.size() == 2);
    REQUIRE(this->renderer.viewportHistory[0].x == 240);
    REQUIRE(this->renderer.viewportHistory[0].y == 0);
    REQUIRE(this->renderer.viewportHistory[0].width == 1440);
    REQUIRE(this->renderer.viewportHistory[0].height == 1080);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem draws sprite using transformed world coordinates",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastDrawX == 415);
    REQUIRE(this->renderer.lastDrawY == 330);
    REQUIRE(this->renderer.lastDrawWidth == 48);
    REQUIRE(this->renderer.lastDrawHeight == 36);
    REQUIRE(this->renderer.lastDrawRotation == 0.0f);
    REQUIRE(this->renderer.lastDrawFlipX == false);
    REQUIRE(this->renderer.lastDrawFlipY == false);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem forwards rotation and flip flags to renderer",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, -2.0f, -3.0f, 37.5f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastDrawWidth == 48);
    REQUIRE(this->renderer.lastDrawHeight == 36);
    REQUIRE(this->renderer.lastDrawRotation == 37.5f);
    REQUIRE(this->renderer.lastDrawFlipX == true);
    REQUIRE(this->renderer.lastDrawFlipY == true);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem forwards sprite source rect to renderer",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ std::make_shared<StubTexture>(), 16, 8, 4, 6, 8, 10, true });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 1);
    REQUIRE(this->renderer.lastUseSourceRect == true);
    REQUIRE(this->renderer.lastSrcX == 4);
    REQUIRE(this->renderer.lastSrcY == 6);
    REQUIRE(this->renderer.lastSrcWidth == 8);
    REQUIRE(this->renderer.lastSrcHeight == 10);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem skips sprites without textures",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<SpriteComponent>(entity, SpriteComponent{ nullptr, 16, 8 });
    components.add<RenderComponent>(entity, RenderComponent{ 0 });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawTextureCalls == 0);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem draws filled rectangle shapes",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    auto shape = std::make_unique<RectangleDef>();
    shape->width = 12.0f;
    shape->height = 8.0f;

    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    components.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{ 1, 2, 3, 4 }, true });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawRectFilledCalls == 1);
    REQUIRE(this->renderer.drawRectOutlineCalls == 0);
    REQUIRE(this->renderer.lastRect.position.x == 398.0f);
    REQUIRE(this->renderer.lastRect.position.y == 308.0f);
    REQUIRE(this->renderer.lastRect.width == 24.0f);
    REQUIRE(this->renderer.lastRect.height == 24.0f);
    REQUIRE(this->renderer.lastColor.r == 1);
    REQUIRE(this->renderer.lastColor.g == 2);
    REQUIRE(this->renderer.lastColor.b == 3);
    REQUIRE(this->renderer.lastColor.a == 4);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem draws outlined circle shapes",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();
    auto& components = this->world.components();

    auto shape = std::make_unique<CircleDef>();
    shape->radius = 7.0f;

    this->camera.setZoom(2.0f);
    components.add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, -3.0f, 0.0f });
    components.add<ShapeRenderComponent>(entity, ShapeRenderComponent{ std::move(shape), Color{ 9, 8, 7, 6 }, false });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawCircleOutlineCalls == 1);
    REQUIRE(this->renderer.drawCircleFilledCalls == 0);
    REQUIRE(this->renderer.lastCircle.position.x == 420.0f);
    REQUIRE(this->renderer.lastCircle.position.y == 340.0f);
    REQUIRE(this->renderer.lastCircle.radius == 42.0f);
    REQUIRE(this->renderer.lastColor.r == 9);
    REQUIRE(this->renderer.lastColor.g == 8);
    REQUIRE(this->renderer.lastColor.b == 7);
    REQUIRE(this->renderer.lastColor.a == 6);
}
