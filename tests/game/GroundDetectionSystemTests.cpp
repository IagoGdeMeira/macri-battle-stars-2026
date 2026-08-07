#include "game/include/GroundDetectionSystem/GroundDetectionSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/CollisionEvent.h"

#include <catch2/catch_test_macros.hpp>

class GroundDetectionSystemFixture
{
public:
    GroundDetectionSystemFixture() : system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<GroundedComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<VelocityComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    GroundDetectionSystem system;
    UpdateContext context;

    Entity createStaticGround(float x, float y)
    {
        const Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{4.f, 4.f });
        comp.add<PushboxComponent>(entity, PushboxComponent{PushboxComponent::Type::Static, 1.f, 1.f});
        comp.add<ActiveComponent>(entity, ActiveComponent{true});
        return entity;
    }
};

TEST_CASE_METHOD(GroundDetectionSystemFixture, "GroundDetectionSystem marks the grounded owner when a child collider stands on a static pushbox",
    "[unit][ground_detection_system]"
) {
    const Entity owner = this->world.entities().create();
    const Entity collider = this->world.entities().create();
    const Entity ground = this->createStaticGround(0.f, 1.f);

    auto& comp = this->world.components();
    comp.add<GroundedComponent>(owner, GroundedComponent{ false, 0.f });
    comp.add<TransformComponent>(owner, TransformComponent{ 0.f, 0.f, 1.f, 1.f, 0.f });
    comp.add<ParentComponent>(collider, ParentComponent{ owner });
    comp.add<RectangleColliderComponent>(collider, RectangleColliderComponent{ 4.f, 4.f });
    comp.add<ActiveComponent>(collider, ActiveComponent{true});

    this->bus.emit<CollisionEvent>(CollisionEvent{ collider, ground });
    this->system.update(this->context);

    REQUIRE(comp.get<GroundedComponent>(owner).onGround);
}

TEST_CASE_METHOD(GroundDetectionSystemFixture, "GroundDetectionSystem ignores side collisions that are not standing contacts",
    "[unit][ground_detection_system]"
) {
    const Entity owner = this->world.entities().create();
    const Entity collider = this->world.entities().create();
    const Entity wall = this->createStaticGround(3.f, 0.f);

    auto& comp = this->world.components();
    comp.add<GroundedComponent>(owner, GroundedComponent{ false, 0.f });
    comp.add<TransformComponent>(owner, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    comp.add<ParentComponent>(collider, ParentComponent{ owner });
    comp.add<RectangleColliderComponent>(collider, RectangleColliderComponent{4.f, 4.f});
    comp.add<ActiveComponent>(collider, ActiveComponent{true});

    this->bus.emit<CollisionEvent>(CollisionEvent{ collider, wall });
    this->system.update(this->context);

    REQUIRE_FALSE(comp.get<GroundedComponent>(owner).onGround);
}
