#include "game/include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"

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

class DynamicPushboxResolutionSystemFixture
{
public:
    DynamicPushboxResolutionSystemFixture() : system(this->bus), context { this->world, this->bus, this->commandBuffer, 0.f }
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
    DynamicPushboxResolutionSystem system;
    UpdateContext context;

    Entity createDynamicRect(float x, float y, float vx, float vy)
    {
        const Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{4.f, 4.f});
        comp.add<PushboxComponent>(entity, PushboxComponent{PushboxComponent::Type::Dynamic, 1.f, 1.f});
        comp.add<VelocityComponent>(entity, VelocityComponent{vx, vy});
        return entity;
    }
};

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture,
    "DynamicPushboxResolutionSystem separates overlapping dynamic pushboxes along the X axis",
    "[unit][dynamic_pushbox_resolution_system]"
) {
    const Entity left = this->createDynamicRect(0.f, 0.f, 3.f, 0.f);
    const Entity right = this->createDynamicRect(1.f, 0.f, -2.f, 0.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ left, right });
    this->system.update(this->context);

    auto& comp = this->world.components();
    const auto& leftTransform = comp.get<TransformComponent>(left);
    const auto& rightTransform = comp.get<TransformComponent>(right);
    const auto& leftVelocity = comp.get<VelocityComponent>(left);
    const auto& rightVelocity = comp.get<VelocityComponent>(right);

    REQUIRE(leftTransform.position.x == Catch::Approx(-1.5f));
    REQUIRE(rightTransform.position.x == Catch::Approx(2.5f));
    REQUIRE(leftVelocity.velocity.x == 0.f);
    REQUIRE(rightVelocity.velocity.x == 0.f);
}

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture,
    "DynamicPushboxResolutionSystem separates overlapping dynamic pushboxes along the Y axis",
    "[unit][dynamic_pushbox_resolution_system]"
) {
    const Entity top = this->createDynamicRect(0.f, 0.f, 0.f, 5.f);
    const Entity bottom = this->createDynamicRect(0.f, 1.f, 0.f, -1.f);

    this->bus.emit<CollisionEvent>(CollisionEvent{ top, bottom });
    this->system.update(this->context);

    auto& comp = this->world.components();
    const auto& topTransform = comp.get<TransformComponent>(top);
    const auto& bottomTransform = comp.get<TransformComponent>(bottom);
    const auto& topVelocity = comp.get<VelocityComponent>(top);
    const auto& bottomVelocity = comp.get<VelocityComponent>(bottom);

    REQUIRE(topTransform.position.y == Catch::Approx(-1.5f));
    REQUIRE(bottomTransform.position.y == Catch::Approx(2.5f));
    REQUIRE(topVelocity.velocity.y == 0.f);
    REQUIRE(bottomVelocity.velocity.y == 0.f);
}

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture,
    "DynamicPushboxResolutionSystem moves root entities when colliders are children",
    "[unit][dynamic_pushbox_resolution_system]")
{
    auto& comp = this->world.components();
    comp.registerComponent<ParentComponent>();

    Entity rootA = this->world.entities().create();
    comp.add<TransformComponent>(rootA, TransformComponent{0.f, 0.f});
    comp.add<VelocityComponent>(rootA, VelocityComponent{3.f, 0.f});

    Entity rootB = this->world.entities().create();
    comp.add<TransformComponent>(rootB, TransformComponent{2.f, 0.f});
    comp.add<VelocityComponent>(rootB, VelocityComponent{-2.f, 0.f});

    Entity childA = this->world.entities().create();
    comp.add<ParentComponent>(childA, ParentComponent{rootA});
    comp.add<TransformComponent>(childA, TransformComponent{});
    comp.add<RectangleColliderComponent>(childA, RectangleColliderComponent{4.f, 4.f});
    comp.add<PushboxComponent>(childA, PushboxComponent{PushboxComponent::Type::Dynamic, 1.f, 1.f});

    Entity childB = this->world.entities().create();
    comp.add<ParentComponent>(childB, ParentComponent{rootB});
    comp.add<TransformComponent>(childB, TransformComponent{});
    comp.add<RectangleColliderComponent>(childB, RectangleColliderComponent{4.f, 4.f});
    comp.add<PushboxComponent>(childB, PushboxComponent{PushboxComponent::Type::Dynamic, 1.f, 1.f});

    comp.get<TransformComponent>(childA).position = {0.f, 0.f};
    comp.get<TransformComponent>(childB).position = {1.f, 0.f};

    this->bus.emit<CollisionEvent>(CollisionEvent{ childA, childB });
    this->system.update(this->context);

    const auto& transA = comp.get<TransformComponent>(rootA);
    const auto& transB = comp.get<TransformComponent>(rootB);

    REQUIRE(transA.position.x == Catch::Approx(-1.5f));
    REQUIRE(transB.position.x == Catch::Approx(3.5f));

    const auto& childTransA = comp.get<TransformComponent>(childA);
    const auto& childTransB = comp.get<TransformComponent>(childB);
    REQUIRE(childTransA.position.x == Catch::Approx(0.f));
    REQUIRE(childTransB.position.x == Catch::Approx(1.f));

    REQUIRE(comp.get<VelocityComponent>(rootA).velocity.x == 0.f);
    REQUIRE(comp.get<VelocityComponent>(rootB).velocity.x == 0.f);
}

TEST_CASE_METHOD(DynamicPushboxResolutionSystemFixture, "DynamicPushboxResolutionSystem does not move child entities directly",
    "[unit][dynamic_pushbox_resolution_system]")
{
    auto& comp = this->world.components();
    comp.registerComponent<ParentComponent>();

    Entity root = this->world.entities().create();
    comp.add<TransformComponent>(root, TransformComponent{5.f, 5.f});
    comp.add<VelocityComponent>(root, VelocityComponent{1.f, 0.f});

    Entity child = this->world.entities().create();
    comp.add<ParentComponent>(child, ParentComponent{root});
    comp.add<TransformComponent>(child, TransformComponent{0.f, 0.f});
    comp.add<RectangleColliderComponent>(child, RectangleColliderComponent{4.f, 4.f});
    comp.add<PushboxComponent>(child, PushboxComponent{PushboxComponent::Type::Dynamic, 1.f, 1.f});

    Entity other = this->createDynamicRect(7.f, 5.f, -2.f, 0.f);

    comp.get<TransformComponent>(child).position = {6.f, 5.f};

    this->bus.emit<CollisionEvent>(CollisionEvent{ child, other });
    this->system.update(this->context);

    REQUIRE(comp.get<TransformComponent>(child).position.x == Catch::Approx(6.f));
    const auto& rootTrans = comp.get<TransformComponent>(root);
    REQUIRE(rootTrans.position.x != Catch::Approx(5.f));
}
