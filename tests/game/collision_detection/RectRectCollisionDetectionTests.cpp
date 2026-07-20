#include "game/collision_detections/RectRectCollisionDetection.h"

#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/CollisionEvent.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class RectRectCollisionDetectionFixture
{
public:
    RectRectCollisionDetectionFixture() : scene(this->bus)
    {
        auto& comp = this->scene.world().components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RectangleColliderComponent>();

        this->bus.subscribe<CollisionEvent>([&](const CollisionEvent& event) { this->events.push_back(event); });
    }

protected:
    EventBus bus;
    Scene scene;
    std::vector<CollisionEvent> events;
    RectRectCollisionDetection detector;

    Entity createRect(float x, float y, float width, float height)
    {
        const Entity entity = this->scene.world().entities().create();
        auto& comp = this->scene.world().components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{width, height});
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

TEST_CASE_METHOD(RectRectCollisionDetectionFixture, "RectRectCollisionDetection detects overlapping rectangles",
    "[unit][rect_rect_collision]"
) {
    Entity a = this->createRect(0.f, 0.f, 4.f, 4.f);
    Entity b = this->createRect(1.f, 0.f, 4.f, 4.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
    REQUIRE(this->hasPair(a, b));
}

TEST_CASE_METHOD(RectRectCollisionDetectionFixture, "RectRectCollisionDetection does not detect rectangles that only touch edges",
    "[unit][rect_rect_collision]"
) {
    Entity a = this->createRect(0.f, 0.f, 2.f, 2.f);
    Entity b = this->createRect(2.f, 0.f, 2.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}

TEST_CASE_METHOD(RectRectCollisionDetectionFixture, "RectRectCollisionDetection detects rectangles with partial overlap",
    "[unit][rect_rect_collision]"
) {
    Entity a = this->createRect(-1.f, -1.f, 3.f, 3.f);
    Entity b = this->createRect(0.f, 0.f, 2.f, 2.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.size() == 1);
}

TEST_CASE_METHOD(RectRectCollisionDetectionFixture, "RectRectCollisionDetection ignores entities without RectangleColliderComponent",
    "[unit][rect_rect_collision]"
) {
    Entity a = this->scene.world().entities().create();
    auto& comp = this->scene.world().components();
    comp.add<TransformComponent>(a, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});

    Entity b = this->createRect(1.f, 0.f, 4.f, 4.f);

    std::vector<ICollisionDetection::CollisionPair> pairs{{a, b}};
    this->runDetection(pairs);

    REQUIRE(this->events.empty());
}
