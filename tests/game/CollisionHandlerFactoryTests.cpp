#include "game/include/CollisionHandlerFactory/CollisionHandlerFactory.h"

#include "domain/components/CircleColliderComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class CollisionHandlerFactoryFixture
{
public:
    CollisionHandlerFactoryFixture() : context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext context;

    Entity createEntity(float x, float y)
    {
        const Entity entity = this->world.entities().create();
        this->world.components().add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        return entity;
    }
};

TEST_CASE_METHOD(CollisionHandlerFactoryFixture,
    "CollisionHandlerFactory prefers rectangle colliders when both shapes are present",
    "[unit][collision_handler_factory]"
) {
    const Entity entity = this->createEntity(5.f, 7.f);
    auto& comp = this->world.components();
    comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{10.f, 6.f});
    comp.add<CircleColliderComponent>(entity, CircleColliderComponent{1.f});

    auto handler = CollisionHandlerFactory::createForEntity(this->context, { entity, std::nullopt });
    REQUIRE(handler);

    const auto aabb = handler->getAABB(this->context, { entity, std::nullopt });
    REQUIRE(aabb.left == Catch::Approx(0.f));
    REQUIRE(aabb.right == Catch::Approx(10.f));
    REQUIRE(aabb.top == Catch::Approx(4.f));
    REQUIRE(aabb.bottom == Catch::Approx(10.f));
}

TEST_CASE_METHOD(CollisionHandlerFactoryFixture,
    "CollisionHandlerFactory falls back to the secondary entity when the preferred entity has no collider",
    "[unit][collision_handler_factory]"
) {
    const Entity preferred = this->world.entities().create();
    const Entity fallback = this->createEntity(3.f, 4.f);
    this->world.components().add<CircleColliderComponent>(fallback, CircleColliderComponent{4.f});

    auto handler = CollisionHandlerFactory::createForEntity(this->context, { preferred, fallback });
    REQUIRE(handler);

    const auto aabb = handler->getAABB(this->context, { preferred, fallback });
    REQUIRE(aabb.left == Catch::Approx(-1.f));
    REQUIRE(aabb.right == Catch::Approx(7.f));
    REQUIRE(aabb.top == Catch::Approx(0.f));
    REQUIRE(aabb.bottom == Catch::Approx(8.f));
}
