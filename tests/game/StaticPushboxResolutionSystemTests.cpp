#include "game/include/StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"

#include "domain/components/CircleColliderComponent.h"
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

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class StaticPushboxResolutionSystemFixture
{
public:
    StaticPushboxResolutionSystemFixture() : system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<CircleColliderComponent>();
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
    StaticPushboxResolutionSystem system;
    UpdateContext context;

    Entity createRect(float x, float y, PushboxComponent::Type type, float vx, float vy)
    {
        const Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{4.f, 4.f});
        comp.add<PushboxComponent>(entity, PushboxComponent{type, 1.f, 1.f});
        comp.add<VelocityComponent>(entity, VelocityComponent{vx, vy});
        return entity;
    }
};

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem pushes a dynamic entity out of a static collider along the X axis",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.f, 0.f, PushboxComponent::Type::Dynamic, 4.f, 0.f);
    const Entity staticEntity = this->createRect(1.f, 0.f, PushboxComponent::Type::Static, 0.f, 0.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& comp = this->world.components();
    const auto& dynamicTransform = comp.get<TransformComponent>(dynamicEntity);
    const auto& staticTransform = comp.get<TransformComponent>(staticEntity);
    const auto& velocity = comp.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.position.x == Catch::Approx(-3.f));
    REQUIRE(staticTransform.position.x == Catch::Approx(1.f));
    REQUIRE(velocity.velocity.x == 0.f);
}

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture,
    "StaticPushboxResolutionSystem ignores static colliders below dynamic entity (ground)",
    "[unit][static_pushbox_resolution_system]"
) {
    const Entity dynamicEntity = this->createRect(0.f, 0.f, PushboxComponent::Type::Dynamic, 0.f, 6.f);
    const Entity staticEntity = this->createRect(0.f, 1.f, PushboxComponent::Type::Static, 0.f, 0.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ dynamicEntity, staticEntity });
    this->system.update(this->context);

    auto& comp = this->world.components();
    const auto& dynamicTransform = comp.get<TransformComponent>(dynamicEntity);
    const auto& velocity = comp.get<VelocityComponent>(dynamicEntity);

    REQUIRE(dynamicTransform.position.y == Catch::Approx(0.f));
    REQUIRE(velocity.velocity.y == 6.f);
}

TEST_CASE_METHOD(StaticPushboxResolutionSystemFixture, "StaticPushboxResolutionSystem moves root entity when dynamic collider is a child",
    "[unit][static_pushbox_resolution_system]")
{
    auto& comp = this->world.components();
    comp.registerComponent<ParentComponent>();

    Entity root = this->world.entities().create();
    comp.add<TransformComponent>(root, TransformComponent{0.f, 0.f});
    comp.add<VelocityComponent>(root, VelocityComponent{4.f, 0.f});

    Entity child = this->world.entities().create();
    comp.add<ParentComponent>(child, ParentComponent{root});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<RectangleColliderComponent>(child, RectangleColliderComponent{4.f, 4.f});
    comp.add<PushboxComponent>(child, PushboxComponent{PushboxComponent::Type::Dynamic, 1.f, 1.f});

    Entity staticEntity = this->createRect(2.f, 0.f, PushboxComponent::Type::Static, 0.f, 0.f);

    comp.get<TransformComponent>(child).position = {1.f, 0.f};

    this->bus.emit<CollisionEvent>(CollisionEvent{ child, staticEntity });
    this->system.update(this->context);

    const auto& rootTrans = comp.get<TransformComponent>(root);
    REQUIRE(rootTrans.position.x == Catch::Approx(-3.f));

    const auto& childTrans = comp.get<TransformComponent>(child);
    REQUIRE(childTrans.position.x == Catch::Approx(1.f));

    REQUIRE(comp.get<VelocityComponent>(root).velocity.x == 0.f);
}
