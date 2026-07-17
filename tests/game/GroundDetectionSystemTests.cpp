#include "../../src/game/include/GroundDetectionSystem/GroundDetectionSystem.h"

#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/value_objects/UpdateContext/UpdateContext.h"

#include "../../src/game/events/CollisionEvent.h"

#include <catch2/catch_test_macros.hpp>

class GroundDetectionSystemFixture
{
public:
    GroundDetectionSystemFixture() : system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& components = this->world.components();
        components.registerComponent<GroundedComponent>();
        components.registerComponent<ParentComponent>();
        components.registerComponent<PushboxComponent>();
        components.registerComponent<RectangleColliderComponent>();
        components.registerComponent<TransformComponent>();
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
        auto& components = this->world.components();
        components.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        components.add<RectangleColliderComponent>(entity, RectangleColliderComponent{4.f, 4.f });
        components.add<PushboxComponent>(entity, PushboxComponent{PushboxComponent::PushboxType::Static, 1.f, 1.f});
        return entity;
    }
};

TEST_CASE_METHOD(GroundDetectionSystemFixture,
    "GroundDetectionSystem marks the grounded owner when a child collider stands on a static pushbox",
    "[unit][ground_detection_system]"
) {
    const Entity owner = this->world.entities().create();
    const Entity collider = this->world.entities().create();
    const Entity ground = this->createStaticGround(0.f, 1.f);

    auto& components = this->world.components();
    components.add<GroundedComponent>(owner, GroundedComponent{ false, 0.f });
    components.add<TransformComponent>(owner, TransformComponent{ 0.f, 0.f, 1.f, 1.f, 0.f });
    components.add<ParentComponent>(collider, ParentComponent{ owner });
    components.add<RectangleColliderComponent>(collider, RectangleColliderComponent{ 4.f, 4.f });

    this->bus.emit<CollisionEvent>(CollisionEvent{ collider, ground });
    this->system.update(this->context);

    REQUIRE(components.get<GroundedComponent>(owner).onGround);
}

TEST_CASE_METHOD(GroundDetectionSystemFixture,
    "GroundDetectionSystem ignores side collisions that are not standing contacts",
    "[unit][ground_detection_system]"
) {
    const Entity owner = this->world.entities().create();
    const Entity collider = this->world.entities().create();
    const Entity wall = this->createStaticGround(3.f, 0.f);

    auto& components = this->world.components();
    components.add<GroundedComponent>(owner, GroundedComponent{ false, 0.f });
    components.add<TransformComponent>(owner, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    components.add<ParentComponent>(collider, ParentComponent{ owner });
    components.add<RectangleColliderComponent>(collider, RectangleColliderComponent{4.f, 4.f});

    this->bus.emit<CollisionEvent>(CollisionEvent{ collider, wall });
    this->system.update(this->context);

    REQUIRE_FALSE(components.get<GroundedComponent>(owner).onGround);
}
