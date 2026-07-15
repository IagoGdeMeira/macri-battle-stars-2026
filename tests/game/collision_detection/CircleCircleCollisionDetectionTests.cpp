#include "../../../src/game/collision_detections/CircleCircleCollisionDetection.h"

#include "../../../src/domain/components/CircleColliderComponent.h"
#include "../../../src/domain/components/TransformComponent.h"

#include "../../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/Scene/Scene.h"
#include "../../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../../src/game/events/CollisionEvent.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class CircleCircleCollisionDetectionFixture
{
public:
    CircleCircleCollisionDetectionFixture() : scene(this->bus)
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<CircleColliderComponent>();

        this->bus.subscribe<CollisionEvent>([&](const CollisionEvent& event) { this->events.push_back(event); });
    } 

protected:
    EventBus bus;
    Scene scene;
    std::vector<CollisionEvent> events;
    CircleCircleCollisionDetection detector;

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

TEST_CASE_METHOD(CircleCircleCollisionDetectionFixture, "CircleCircleCollisionDetection detects overlapping circles",
    "[unit][circle_circle_collision]"
) {
    Entity a = this->createCircle(0.f, 0.f, 2.f);
    Entity b = this->createCircle(3.f, 0.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
    REQUIRE(this->hasPair(a, b));
}

TEST_CASE_METHOD(CircleCircleCollisionDetectionFixture, "CircleCircleCollisionDetection does not detect separated circles",
    "[unit][circle_circle_collision]"
) {
    Entity a = this->createCircle(0.f, 0.f, 2.f);
    Entity b = this->createCircle(10.f, 0.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}

TEST_CASE_METHOD(CircleCircleCollisionDetectionFixture, "CircleCircleCollisionDetection detects circles that touch at one point",
    "[unit][circle_circle_collision]"
) {
    Entity a = this->createCircle(0.f, 0.f, 2.f);
    Entity b = this->createCircle(4.f, 0.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
}

TEST_CASE_METHOD(CircleCircleCollisionDetectionFixture, "CircleCircleCollisionDetection ignores entities without CircleColliderComponent",
    "[unit][circle_circle_collision]"
) {
    Entity a = this->scene.world().entities().create();
    auto& comp = this->scene.world().components();
    comp.add<TransformComponent>(a, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});

    Entity b = this->createCircle(1.f, 0.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}
