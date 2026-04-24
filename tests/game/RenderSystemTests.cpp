#include "../../src/game/include/RenderSystem/RenderSystem.h"

#include "../../src/domain/include/World/World.h"

#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/TransformComponent.h"

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
        this->world.components().registerComponent<TransformComponent>();
        this->world.components().registerComponent<SpriteComponent>();
        this->world.components().registerComponent<RenderComponent>();
    }

protected:
    struct StubRenderer : Renderer
    {
        int clearCalls = 0;
        int presentCalls = 0;
        int drawCalls = 0;
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
        int viewportX = 0;
        int viewportY = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;
        std::vector<Viewport> viewportHistory;

        void clear() override { this->clearCalls++; }
        void present() override { this->presentCalls++; }

        std::shared_ptr<Texture> createTexture(const std::string& filePath) override
        {
            (void)filePath;
            return std::make_shared<Texture>();
        }

        void draw(const Texture& texture, const Renderer::DrawParams& params) override
        {
            (void)texture;
            this->drawCalls++;
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

    this->world.components().add<TransformComponent>(
        entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    this->world.components().add<SpriteComponent>(
        entity, SpriteComponent{ std::make_shared<Texture>(), 16, 8 });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawCalls == 1);
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

    this->world.components().add<TransformComponent>(
        entity, TransformComponent{ 10.0f, 20.0f, -2.0f, -3.0f, 37.5f });
    this->world.components().add<SpriteComponent>(
        entity, SpriteComponent{ std::make_shared<Texture>(), 16, 8 });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawCalls == 1);
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

    this->world.components().add<TransformComponent>(
        entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    this->world.components().add<SpriteComponent>(
        entity,
        SpriteComponent{ std::make_shared<Texture>(), 16, 8, 4, 6, 8, 10, true });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawCalls == 1);
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

    this->world.components().add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    this->world.components().add<SpriteComponent>(entity, SpriteComponent{ nullptr, 16, 8 });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->system.draw(this->context);

    REQUIRE(this->renderer.drawCalls == 0);
}
