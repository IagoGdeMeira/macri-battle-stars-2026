#include "../../src/game/include/CameraControllerSystem/CameraControllerSystem.h"

#include "../stubs/StubWindow.h"

#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/Geometry/Geometry.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class CameraControllerSystemFixture
{
public:
    CameraControllerSystemFixture() : ctx { this->world, this->bus, this->commandBuffer, 0.016f }
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
    CameraControllerSystem system(this->camera, this->window);

    this->camera.setPosition(12.f, -8.f);
    this->camera.setZoom(1.25f);

    system.update(this->ctx);

    REQUIRE(this->camera.getPosition().x == 12.f);
    REQUIRE(this->camera.getPosition().y == -8.f);
    REQUIRE(this->camera.getZoom() == 1.25f);
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem centers on player and clamps zoom",
    "[unit][camera_controller_system]"
) {
    const auto entity = this->world.entities().create();
    this->world.components().add<TransformComponent>(entity, TransformComponent { 100.f, 200.f, 1.f, 1.f, 0.f });
    this->world.components().add<PlayerComponent>(entity, PlayerComponent { 1 });

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    CameraControllerSystem system(this->camera, this->window, bounds);

    system.update(this->ctx);

    REQUIRE(this->camera.getPosition().x == 100.f);
    REQUIRE(this->camera.getPosition().y == 200.f);
    REQUIRE(this->camera.getZoom() == 2.f);
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem clamps camera position to map bounds",
    "[unit][camera_controller_system]"
) {
    const auto entityA = this->world.entities().create();
    this->world.components().add<TransformComponent>(entityA, TransformComponent { 100.f, 100.f, 1.f, 1.f, 0.f });
    this->world.components().add<PlayerComponent>(entityA, PlayerComponent { 1 });

    const auto entityB = this->world.entities().create();
    this->world.components().add<TransformComponent>(entityB, TransformComponent { 250.f, 200.f, 1.f, 1.f, 0.f });
    this->world.components().add<PlayerComponent>(entityB, PlayerComponent { 2 });

    AABB bounds { 0.f, 300.f, 0.f, 220.f };
    CameraControllerSystem system(this->camera, this->window, bounds);

    system.update(this->ctx);

    REQUIRE(this->camera.getPosition().x == 150.f);
    REQUIRE(this->camera.getPosition().y == 110.f);
    REQUIRE(this->camera.getZoom() == 2.f);
}
