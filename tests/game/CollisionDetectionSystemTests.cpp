#include "game/include/CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "game/collision_detections/RectRectCollisionDetection.h"
#include "game/collision_detections/CircleCircleCollisionDetection.h"
#include "game/collision_detections/RectCircleCollisionDetection.h"

#include "domain/components/CircleColliderComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"

#include "game/events/CollisionEvent.h"

#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include <memory>
#include <vector>

class CollisionDetectionSystemFixture
{
public:
    CollisionDetectionSystemFixture() : scene(this->bus)
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();

        this->bus.subscribe<CollisionEvent>([&](const CollisionEvent& event) { this->events.push_back(event); });

        auto& system = this->scene.systems().addSystem<CollisionDetectionSystem>(500.f);
        system.addDetector(std::make_unique<RectRectCollisionDetection>());
        system.addDetector(std::make_unique<CircleCircleCollisionDetection>());
        system.addDetector(std::make_unique<RectCircleCollisionDetection>());
    }

protected:
    EventBus bus;
    Scene scene;
    std::vector<CollisionEvent> events;

    Entity createRect(float x, float y, float width, float height)
    {
        const Entity entity = this->scene.world().entities().create();
        auto& comp = this->scene.world().components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{width, height});
        return entity;
    }

    Entity createCircle(float x, float y, float radius)
    {
        const Entity entity = this->scene.world().entities().create();
        auto& comp = this->scene.world().components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<CircleColliderComponent>(entity, CircleColliderComponent{radius});
        return entity;
    }

    void update() { this->scene.update(0.016f); }

    bool hasPair(Entity a, Entity b) const
    {
        return std::any_of(this->events.begin(), this->events.end(), [&](const CollisionEvent& event)
        { return (event.a == a && event.b == b) || (event.a == b && event.b == a); });
    }
};

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem detects rectangle-rectangle collision with grid",
    "[integration][collision_detection_system]"
) {
    const Entity left = this->createRect(0.f, 0.f, 4.f, 4.f);
    const Entity right = this->createRect(1.f, 0.f, 4.f, 4.f);

    this->update();

    REQUIRE(this->hasPair(left, right));
}

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem detects circle-circle collision with grid",
    "[integration][collision_detection_system]"
) {
    const Entity first = this->createCircle(0.f, 0.f, 2.f);
    const Entity second = this->createCircle(3.f, 0.f, 2.f);

    this->update();

    REQUIRE(this->hasPair(first, second));
}

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem detects rect-circle collision with grid",
    "[integration][collision_detection_system]"
) {
    const Entity rect = this->createRect(0.f, 0.f, 4.f, 4.f);
    const Entity circle = this->createCircle(1.5f, 0.f, 1.f);

    this->update();

    REQUIRE(this->hasPair(rect, circle));
}

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem does not emit events for separated entities",
    "[integration][collision_detection_system]"
) {
    this->createRect(-10.f, -10.f, 2.f, 2.f);
    this->createCircle(10.f, 10.f, 1.f);

    this->update();

    REQUIRE(this->events.empty());
}

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem detects collision across grid cell boundary",
    "[integration][collision_detection_system]"
) {
    const Entity rect = this->createRect(499.f, 0.f, 40.f, 20.f);
    const Entity circle = this->createCircle(501.f, 0.f, 2.f);

    this->update();

    REQUIRE(this->hasPair(rect, circle));
}

TEST_CASE_METHOD(CollisionDetectionSystemFixture,
    "CollisionDetectionSystem emits each colliding pair only once across multiple cells",
    "[integration][collision_detection_system]"
) {
    const Entity left = this->createRect(99.f, 0.f, 40.f, 20.f);
    const Entity right = this->createRect(101.f, 0.f, 40.f, 20.f);

    this->update();

    REQUIRE(this->events.size() == 1);
    REQUIRE(this->hasPair(left, right));
}
