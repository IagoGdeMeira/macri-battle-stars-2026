#include "game/include/CameraControllerSystem/CameraControllerSystem.h"

#include "StubWindow.h"

#include "domain/components/PlayerComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class CameraControllerSystemFixture
{
public:
    CameraControllerSystemFixture() : ctx { this->world, this->bus, this->commandBuffer, 0.016f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<PlayerComponent>();
        comp.registerComponent<SpriteComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    StubWindow window;
    Camera2D camera;
    UpdateContext ctx;

    Entity createPlayer(float x, float y, float width = 32.f, float height = 64.f)
    {
        Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<PlayerComponent>(entity, PlayerComponent{1});
        comp.add<SpriteComponent>(entity, SpriteComponent{"dummy.png", nullptr, Dimension2D{width, height}});
        return entity;
    }

    Entity createPlayerNoSprite(float x, float y)
    {
        Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<PlayerComponent>(entity, PlayerComponent{1});
        return entity;
    }

    void clearWorld()
    {
        this->world.entities().clear();
        this->world.components().clear();
    }
};

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem keeps camera unchanged when no players exist",
    "[unit][camera_controller_system]"
) {
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 1.5f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
    });

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
    this->createPlayer(100.f, 200.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 1.5f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(100.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(200.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(1.5f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem clamps camera position to map bounds",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(100.f, 100.f, 32.f, 64.f);
    this->createPlayer(250.f, 200.f, 32.f, 64.f);

    AABB bounds { 0.f, 300.f, 0.f, 220.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 1.5f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(175.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(150.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(1.5f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem uses SpriteComponent size for bounding box",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(0.f, 0.f, 100.f, 100.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 0.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
    REQUIRE(this->camera.getPosition().x == Catch::Approx(0.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem applies padding correctly",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(0.f, 0.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    float padding = 100.f;
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = padding,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
    REQUIRE(this->camera.getPosition().x == Catch::Approx(0.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem handles multiple players with different positions",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(100.f, 100.f, 32.f, 64.f);
    this->createPlayer(300.f, 200.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
    REQUIRE(this->camera.getPosition().x == Catch::Approx(200.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(150.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem uses fallback size when SpriteComponent is absent",
    "[unit][camera_controller_system]"
) {
    this->createPlayerNoSprite(0.f, 0.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 0.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
    REQUIRE(this->camera.getPosition().x == Catch::Approx(0.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem handles infinite bounds (no clamping)",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(1000.f, 1000.f, 32.f, 64.f);

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(1000.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(1000.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem clamps when bounds are finite",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(1000.f, 1000.f, 32.f, 64.f);

    AABB bounds { 0.f, 200.f, 0.f, 200.f };
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(1000.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(1000.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem verticalOffset shifts camera down preserving padding",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(0.f, 0.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    float padding = 50.f, offset = 30.f;

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.f,
        .padding        = padding,
        .verticalOffset = offset,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(0.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(offset));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem verticalOffset works with multiple players",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(100.f, 100.f, 32.f, 64.f);
    this->createPlayer(200.f, 200.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    float padding = 50.f, offset = 30.f;

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.f,
        .padding        = padding,
        .verticalOffset = offset,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(150.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(180.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem verticalOffset preserves padding when players near top",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(0.f, -100.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    float padding = 50.f, offset = 100.f;

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.f,
        .padding        = padding,
        .verticalOffset = offset,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(0.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(0.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem verticalOffset is clamped by bounds",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(0.f, 0.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, 100.f, 400.f };
    float padding = 50.f, offset = -200.f;

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.f,
        .padding        = padding,
        .verticalOffset = offset,
        .bounds         = bounds
    });

    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().y == Catch::Approx(250.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem verticalOffset keeps padding consistent with zoom",
    "[unit][camera_controller_system]"
) {
    this->createPlayer(100.f, 100.f, 32.f, 64.f);
    this->createPlayer(200.f, 200.f, 32.f, 64.f);

    AABB bounds { -1000.f, 1000.f, -1000.f, 1000.f };
    float padding = 50.f, offset = 30.f;

    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.f,
        .padding        = padding,
        .verticalOffset = offset,
        .bounds         = bounds
    });

    system.update(this->ctx);
    auto posY1 = this->camera.getPosition().y;

    system.update(this->ctx);
    auto posY2 = this->camera.getPosition().y;

    REQUIRE(posY1 == Catch::Approx(posY2));
    REQUIRE(posY1 == Catch::Approx(180.f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem does not update when changes are smaller than epsilon",
    "[unit][camera_controller_system]"
) {
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 1.5f,
        .maxZoom        = 1.5f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .epsilon        = 0.01f
    });

    this->camera.setPosition(100.f, 200.f);
    this->camera.setZoom(1.5f);

    this->createPlayer(100.005f, 200.005f, 32.f, 64.f);

    system.update(this->ctx);

    REQUIRE(this->camera.getPosition().x == Catch::Approx(100.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(200.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(1.5f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem updates only when zoom change exceeds epsilon",
    "[unit][camera_controller_system]"
) {
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .epsilon        = 0.01f
    });

    this->camera.setPosition(0.f, 0.f);
    this->camera.setZoom(0.8f);

    this->createPlayer(0.f, 0.f, 1000.f, 1000.f);

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(0.8f));

    this->clearWorld();
    this->createPlayer(0.f, 0.f, 900.f, 900.f);

    system.update(this->ctx);
    REQUIRE(this->camera.getZoom() == Catch::Approx(0.8f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem respects custom epsilon value",
    "[unit][camera_controller_system]"
) {
    const float customEpsilon = 0.1f;
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 1.5f,
        .maxZoom        = 1.5f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .epsilon        = customEpsilon
    });

    this->camera.setPosition(100.f, 200.f);
    this->camera.setZoom(1.5f);

    this->createPlayer(100.05f, 200.05f, 32.f, 64.f);
    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(100.f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(200.f));

    this->clearWorld();
    this->createPlayer(100.2f, 200.2f, 32.f, 64.f);
    system.update(this->ctx);
    REQUIRE(this->camera.getPosition().x == Catch::Approx(100.2f));
    REQUIRE(this->camera.getPosition().y == Catch::Approx(200.2f));
}

TEST_CASE_METHOD(CameraControllerSystemFixture, "CameraControllerSystem updates when changes exceed epsilon",
    "[unit][camera_controller_system]"
) {
    CameraControllerSystem system(CameraControllerSystem::Config{
        .camera         = this->camera,
        .window         = this->window,
        .minZoom        = 0.8f,
        .maxZoom        = 2.0f,
        .padding        = 50.f,
        .verticalOffset = 0.f,
        .epsilon        = 0.01f
    });

    this->camera.setPosition(100.f, 200.f);
    this->camera.setZoom(1.5f);

    this->createPlayer(150.f, 250.f, 32.f, 64.f);

    system.update(this->ctx);

    REQUIRE(this->camera.getPosition().x != Catch::Approx(100.f));
    REQUIRE(this->camera.getPosition().y != Catch::Approx(200.f));
    REQUIRE(this->camera.getZoom() == Catch::Approx(2.0f));
}
