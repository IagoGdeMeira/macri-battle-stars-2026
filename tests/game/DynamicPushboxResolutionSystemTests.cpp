#include "../../src/game/include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"

#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../src/game/events/CollisionEvent.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class DynamicPushboxResolutionSystemFixture
{
public:
    DynamicPushboxResolutionSystemFixture() : system(bus), context { world, bus, commandBuffer, 0.0f }
    {
        auto& components = this->world.components();
        components.registerComponent<TransformComponent>();
        components.registerComponent<RectangleColliderComponent>();
        components.registerComponent<PushboxComponent>();
        components.registerComponent<VelocityComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    DynamicPushboxResolutionSystem system;
    UpdateContext context;

    Entity createDynamicRect(float x, float y, float vx, float vy)
    {
        const Entity entity = this->world.entities().create();
        auto& components = this->world.components();
        components.add<TransformComponent>(entity, TransformComponent{ x, y, 1.0f, 1.0f, 0.0f });
        components.add<RectangleColliderComponent>(entity, RectangleColliderComponent{ 4.0f, 4.0f });
        components.add<PushboxComponent>(entity, PushboxComponent{ PushboxComponent::PushboxType::Dynamic, 1.0f, 1.0f });
        components.add<VelocityComponent>(entity, VelocityComponent{ vx, vy });
        return entity;
    }
};

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture,
    "DynamicPushboxResolutionSystem separates overlapping dynamic pushboxes along the X axis",
    "[unit][dynamic_pushbox_resolution_system]"
) {
    const Entity left = this->createDynamicRect(0.0f, 0.0f, 3.0f, 0.0f);
    const Entity right = this->createDynamicRect(1.0f, 0.0f, -2.0f, 0.0f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ left, right });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& leftTransform = components.get<TransformComponent>(left);
    const auto& rightTransform = components.get<TransformComponent>(right);
    const auto& leftVelocity = components.get<VelocityComponent>(left);
    const auto& rightVelocity = components.get<VelocityComponent>(right);

    REQUIRE(leftTransform.x == Catch::Approx(-1.5f));
    REQUIRE(rightTransform.x == Catch::Approx(2.5f));
    REQUIRE(leftVelocity.vx == 0.0f);
    REQUIRE(rightVelocity.vx == 0.0f);
}

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture,
    "DynamicPushboxResolutionSystem separates overlapping dynamic pushboxes along the Y axis",
    "[unit][dynamic_pushbox_resolution_system]"
) {
    const Entity top = this->createDynamicRect(0.0f, 0.0f, 0.0f, 5.0f);
    const Entity bottom = this->createDynamicRect(0.0f, 1.0f, 0.0f, -1.0f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ top, bottom });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& topTransform = components.get<TransformComponent>(top);
    const auto& bottomTransform = components.get<TransformComponent>(bottom);
    const auto& topVelocity = components.get<VelocityComponent>(top);
    const auto& bottomVelocity = components.get<VelocityComponent>(bottom);

    REQUIRE(topTransform.y == Catch::Approx(-1.5f));
    REQUIRE(bottomTransform.y == Catch::Approx(2.5f));
    REQUIRE(topVelocity.vy == 0.0f);
    REQUIRE(bottomVelocity.vy == 0.0f);
}
