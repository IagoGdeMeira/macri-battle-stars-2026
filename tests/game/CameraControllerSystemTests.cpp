#include "../../src/game/include/CameraControllerSystem/CameraControllerSystem.h"

#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/Geometry/Geometry.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_test_macros.hpp>

class CameraControllerSystemFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int width, int height, const char* /*title*/) override
        {
            this->w = width;
            this->h = height;
        }

        void setResolution(int width, int height) override
        {
            this->w = width;
            this->h = height;
        }

        void setFullscreen(bool enabled) override
        { this->fullscreen = enabled; }

        void getSize(int& width, int& height) override
        {
            width = this->w;
            height = this->h;
        }

    private:
        int w = 800;
        int h = 600;
        bool fullscreen = false;
    };

    CameraControllerSystemFixture() : ctx { world, bus, commandBuffer, 0.016f }
    {
        this->world.components().registerComponent<TransformComponent>();
        this->world.components().registerComponent<PlayerComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    StubWindow window;
    Camera2D camera;
    UpdateContext ctx;
};

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem keeps camera unchanged when no players exist",
    "[unit][camera_controller_system]"
) {
    CameraControllerSystem system(camera, window);

    camera.setPosition(12.0f, -8.0f);
    camera.setZoom(1.25f);

    system.update(ctx);

    REQUIRE(camera.getX() == 12.0f);
    REQUIRE(camera.getY() == -8.0f);
    REQUIRE(camera.getZoom() == 1.25f);
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem centers on player and clamps zoom",
    "[unit][camera_controller_system]"
) {
    const auto entity = this->world.entities().create();
    this->world.components().add<TransformComponent>(entity, TransformComponent { 100.0f, 200.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<PlayerComponent>(entity, PlayerComponent { 1 });

    AABB bounds { -1000.0f, 1000.0f, -1000.0f, 1000.0f };
    CameraControllerSystem system(camera, window, bounds);

    system.update(ctx);

    REQUIRE(camera.getX() == 100.0f);
    REQUIRE(camera.getY() == 200.0f);
    REQUIRE(camera.getZoom() == 2.0f);
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem clamps camera position to map bounds",
    "[unit][camera_controller_system]"
) {
    const auto entityA = this->world.entities().create();
    this->world.components().add<TransformComponent>(entityA, TransformComponent { 100.0f, 100.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<PlayerComponent>(entityA, PlayerComponent { 1 });

    const auto entityB = this->world.entities().create();
    this->world.components().add<TransformComponent>(entityB, TransformComponent { 250.0f, 200.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<PlayerComponent>(entityB, PlayerComponent { 2 });

    AABB bounds { 0.0f, 300.0f, 0.0f, 220.0f };
    CameraControllerSystem system(camera, window, bounds);

    system.update(ctx);

    REQUIRE(camera.getX() == 150.0f);
    REQUIRE(camera.getY() == 110.0f);
    REQUIRE(camera.getZoom() == 2.0f);
}
