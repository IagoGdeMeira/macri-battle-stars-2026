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
    StaticPushboxResolutionSystemFixture() :
        system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.f }
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
        components.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        components.add<RectangleColliderComponent>(entity, RectangleColliderComponent{4.f, 4.f});
        components.add<PushboxComponent>(entity, PushboxComponent{type, 1.f, 1.f});
        components.add<VelocityComponent>(entity, VelocityComponent{vx, vy});
        return entity;
    }
};

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem pushes a dynamic entity out of a static collider along the X axis",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.f, 0.f, PushboxComponent::PushboxType::Dynamic, 4.f, 0.f);
    const Entity staticEntity = this->createRect(1.f, 0.f, PushboxComponent::PushboxType::Static, 0.f, 0.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& dynamicTransform = components.get<TransformComponent>(dynamicEntity);
    const auto& staticTransform = components.get<TransformComponent>(staticEntity);
    const auto& velocity = components.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.position.x == Catch::Approx(-3.f));
    REQUIRE(staticTransform.position.x == Catch::Approx(1.f));
    REQUIRE(velocity.velocity.x == 0.f);
}

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem pushes a dynamic entity out of a static collider along the Y axis",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.f, 0.f, PushboxComponent::PushboxType::Dynamic, 0.f, 6.f);
    const Entity staticEntity = this->createRect(0.f, 1.f, PushboxComponent::PushboxType::Static, 0.f, 0.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& components = this->world.components();
    const auto& dynamicTransform = components.get<TransformComponent>(dynamicEntity);
    const auto& velocity = components.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.position.y == Catch::Approx(-3.f));
    REQUIRE(velocity.velocity.y == 0.f);
}
