#include "../../src/game/include/RenderSystem/RenderSystem.h"

#include "../../src/domain/include/World/World.h"

#include "../../src/engine/events/WindowResizedEvent.h"
#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class RenderSystemFixture
{
public:
    RenderSystemFixture() : system(bus, renderer), context { world, bus, commandBuffer, 0.016f } {}

protected:
    struct StubRenderer : Renderer
    {
        int clearCalls = 0;
        int presentCalls = 0;
        int viewportCalls = 0;
        int viewportX = 0;
        int viewportY = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;

        void clear() override { this->clearCalls++; }
        void present() override { this->presentCalls++; }

        void setViewport(int xPosition, int yPosition, int width, int height) override
        {
            this->viewportCalls++;
            this->viewportX = xPosition;
            this->viewportY = yPosition;
            this->viewportWidth = width;
            this->viewportHeight = height;
        }
    };

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    StubRenderer renderer;
    RenderSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem clears and presents each update",
    "[unit][render_system]"
) {
    this->system.update(this->context);

    REQUIRE(this->renderer.clearCalls == 1);
    REQUIRE(this->renderer.presentCalls == 1);
    REQUIRE(this->renderer.viewportCalls == 0);
}

TEST_CASE_METHOD(RenderSystemFixture, "RenderSystem updates viewport on window resize",
    "[unit][render_system]"
) {
    this->bus.emit<WindowResizedEvent>(WindowResizedEvent { 1920, 1080 });

    REQUIRE(this->renderer.viewportCalls == 1);
    REQUIRE(this->renderer.viewportX == 240);
    REQUIRE(this->renderer.viewportY == 0);
    REQUIRE(this->renderer.viewportWidth == 1440);
    REQUIRE(this->renderer.viewportHeight == 1080);
}
