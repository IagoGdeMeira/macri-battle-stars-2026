#include "../../src/game/include/StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"

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

class StaticPushboxResolutionSystemFixture
{
public:
    StaticPushboxResolutionSystemFixture() : system(bus), context { world, bus, commandBuffer, 0.0f }
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
    StaticPushboxResolutionSystem system;
    UpdateContext context;

    Entity createRect(float x, float y, PushboxComponent::PushboxType type, float vx, float vy)
    {
        const Entity entity = this->world.entities().create();
        auto& components = this->world.components();
        components.add<TransformComponent>(entity, TransformComponent{ x, y, 1.0f, 1.0f, 0.0f });
        components.add<RectangleColliderComponent>(entity, RectangleColliderComponent{ 4.0f, 4.0f });
        components.add<PushboxComponent>(entity, PushboxComponent{ type, 1.0f, 1.0f });
        components.add<VelocityComponent>(entity, VelocityComponent{ vx, vy });
        return entity;
    }
};

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem pushes a dynamic entity out of a static collider along the X axis",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.0f, 0.0f, PushboxComponent::PushboxType::Dynamic, 4.0f, 0.0f);
    const Entity staticEntity = this->createRect(1.0f, 0.0f, PushboxComponent::PushboxType::Static, 0.0f, 0.0f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& dynamicTransform = components.get<TransformComponent>(dynamicEntity);
    const auto& staticTransform = components.get<TransformComponent>(staticEntity);
    const auto& velocity = components.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.x == Catch::Approx(-3.0f));
    REQUIRE(staticTransform.x == Catch::Approx(1.0f));
    REQUIRE(velocity.vx == 0.0f);
}

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem pushes a dynamic entity out of a static collider along the Y axis",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.0f, 0.0f, PushboxComponent::PushboxType::Dynamic, 0.0f, 6.0f);
    const Entity staticEntity = this->createRect(0.0f, 1.0f, PushboxComponent::PushboxType::Static, 0.0f, 0.0f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& dynamicTransform = components.get<TransformComponent>(dynamicEntity);
    const auto& velocity = components.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.y == Catch::Approx(-3.0f));
    REQUIRE(velocity.vy == 0.0f);
}
