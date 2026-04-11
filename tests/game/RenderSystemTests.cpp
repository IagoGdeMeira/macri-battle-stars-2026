#include "../../src/game/include/RenderSystem/RenderSystem.h"

#include "../../src/domain/include/World/World.h"

#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/TransformComponent.h"

#include "../../src/engine/events/WindowResizedEvent.h"
#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../src/game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class RenderSystemFixture
{
public:
    RenderSystemFixture() : system(bus, renderer, camera), context { world, bus, commandBuffer, 0.016f }
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

        void draw(const Texture& texture, int x, int y, int width, int height) override
        {
            (void)texture;
            this->drawCalls++;
            this->lastDrawX = x;
            this->lastDrawY = y;
            this->lastDrawWidth = width;
            this->lastDrawHeight = height;
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
    CommandBuffer commandBuffer;
    StubRenderer renderer;
    Camera2D camera;
    RenderSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem clears and presents each update",
    "[unit][render_system]"
) {
    this->system.update(this->context);

    REQUIRE(this->renderer.clearCalls == 1);
    REQUIRE(this->renderer.presentCalls == 1);
    REQUIRE(this->renderer.viewportCalls == 2);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem updates viewport on window resize",
    "[unit][render_system]"
) {
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });

    this->system.update(this->context);

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

    this->world.components().add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    this->world.components().add<SpriteComponent>(
        entity, SpriteComponent{ std::make_shared<Texture>(), 16, 8 });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->camera.setPosition(0.0f, 0.0f);
    this->camera.setZoom(1.5f);

    this->system.update(this->context);

    REQUIRE(this->renderer.drawCalls == 1);
    REQUIRE(this->renderer.lastDrawX == 415);
    REQUIRE(this->renderer.lastDrawY == 330);
    REQUIRE(this->renderer.lastDrawWidth == 48);
    REQUIRE(this->renderer.lastDrawHeight == 36);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem skips sprites without textures",
    "[unit][render_system]"
) {
    const auto entity = this->world.entities().create();

    this->world.components().add<TransformComponent>(entity, TransformComponent{ 10.0f, 20.0f, 2.0f, 3.0f, 0.0f });
    this->world.components().add<SpriteComponent>(entity, SpriteComponent{ nullptr, 16, 8 });
    this->world.components().add<RenderComponent>(entity, RenderComponent{ 0 });

    this->system.update(this->context);

    REQUIRE(this->renderer.drawCalls == 0);
}
