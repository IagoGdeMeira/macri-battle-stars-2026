#include "../../../src/game/collision_detections/RectCircleCollisionDetection.h"

#include "../../../src/domain/components/CircleColliderComponent.h"
#include "../../../src/domain/components/RectangleColliderComponent.h"
#include "../../../src/domain/components/TransformComponent.h"

#include "../../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/Scene/Scene.h"
#include "../../../src/engine/value_objects/UpdateContext/UpdateContext.h"

#include "../../../src/game/events/CollisionEvent.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class RectCircleCollisionDetectionFixture
{
public:
    RectCircleCollisionDetectionFixture() : scene(this->bus)
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();

        this->bus.subscribe<CollisionEvent>([&](const CollisionEvent& event) { this->events.push_back(event); });
    }

protected:
    EventBus bus;
    Scene scene;
    std::vector<CollisionEvent> events;
    RectCircleCollisionDetection detector;

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

    void runDetection(const std::vector<ICollisionDetection::CollisionPair>& pairs)
    {
        UpdateContext ctx{this->scene.world(), this->bus, this->commandBuffer, 0.016f};
        this->detector.detect(pairs, ctx);
    }

    bool hasPair(Entity a, Entity b) const
    {
        return std::any_of(this->events.begin(), this->events.end(), [&](const CollisionEvent& event)
        { return (event.a == a && event.b == b) || (event.a == b && event.b == a); });
    }

private:
    CommandBuffer commandBuffer;
};

TEST_CASE_METHOD(RectCircleCollisionDetectionFixture, "RectCircleCollisionDetection detects rectangle and circle overlap",
    "[unit][rect_circle_collision]"
) {
    Entity rect = this->createRect(0.f, 0.f, 4.f, 4.f);
    Entity circle = this->createCircle(1.5f, 0.f, 1.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{rect, circle}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
    REQUIRE(this->hasPair(rect, circle));
}

TEST_CASE_METHOD(RectCircleCollisionDetectionFixture, "RectCircleCollisionDetection detects rectangle and circle touching edge",
    "[unit][rect_circle_collision]"
) {
    Entity rect = this->createRect(0.f, 0.f, 4.f, 4.f);
    Entity circle = this->createCircle(2.f, 0.f, 1.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{rect, circle}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
}

TEST_CASE_METHOD(RectCircleCollisionDetectionFixture, "RectCircleCollisionDetection does not detect separated rect and circle",
    "[unit][rect_circle_collision]"
) {
    Entity rect = this->createRect(0.f, 0.f, 2.f, 2.f);
    Entity circle = this->createCircle(10.f, 10.f, 1.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{rect, circle}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}

TEST_CASE_METHOD(RectCircleCollisionDetectionFixture, "RectCircleCollisionDetection handles order inversion (circle first, rect second)",
    "[unit][rect_circle_collision]"
) {
    Entity rect = this->createRect(0.f, 0.f, 4.f, 4.f);
    Entity circle = this->createCircle(1.5f, 0.f, 1.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{circle, rect}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
    REQUIRE(this->hasPair(circle, rect));
}

TEST_CASE_METHOD(RectCircleCollisionDetectionFixture, "RectCircleCollisionDetection ignores entities without the correct components",
    "[unit][rect_circle_collision]"
) {
    Entity a = this->scene.world().entities().create();
    auto& comp = this->scene.world().components();
    comp.add<TransformComponent>(a, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});

    Entity circle = this->createCircle(1.f, 0.f, 1.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, circle}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}
