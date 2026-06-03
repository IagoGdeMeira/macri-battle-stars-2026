#include "../../src/game/include/CollisionHandler/CollisionHandlerFactory.h"

#include "../../src/domain/components/CircleColliderComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class CollisionHandlerFactoryFixture
{
public:
    CollisionHandlerFactoryFixture() : context { world, bus, commandBuffer, 0.0f }
    {
        auto& components = this->world.components();
        components.registerComponent<TransformComponent>();
        components.registerComponent<RectangleColliderComponent>();
        components.registerComponent<CircleColliderComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext context;

    Entity createEntity(float x, float y)
    {
        const Entity entity = this->world.entities().create();
        this->world.components().add<TransformComponent>(entity, TransformComponent{ x, y, 1.0f, 1.0f, 0.0f });
        return entity;
    }
};

TEST_CASE_METHOD(CollisionHandlerFactoryFixture,
    "CollisionHandlerFactory prefers rectangle colliders when both shapes are present",
    "[unit][collision_handler_factory]"
) {
    const Entity entity = this->createEntity(5.0f, 7.0f);
    this->world.components().add<RectangleColliderComponent>(entity, RectangleColliderComponent{ 10.0f, 6.0f });
    this->world.components().add<CircleColliderComponent>(entity, CircleColliderComponent{ 1.0f });

    auto handler = CollisionHandlerFactory::createForEntity(this->context, { entity, std::nullopt });
    REQUIRE(handler);

    const auto aabb = handler->getAABB(this->context, { entity, std::nullopt });
    REQUIRE(aabb.left == Catch::Approx(0.0f));
    REQUIRE(aabb.right == Catch::Approx(10.0f));
    REQUIRE(aabb.top == Catch::Approx(4.0f));
    REQUIRE(aabb.bottom == Catch::Approx(10.0f));
}

TEST_CASE_METHOD(CollisionHandlerFactoryFixture,
    "CollisionHandlerFactory falls back to the secondary entity when the preferred entity has no collider",
    "[unit][collision_handler_factory]"
) {
    const Entity preferred = this->world.entities().create();
    const Entity fallback = this->createEntity(3.0f, 4.0f);
    this->world.components().add<CircleColliderComponent>(fallback, CircleColliderComponent{ 4.0f });

    auto handler = CollisionHandlerFactory::createForEntity(this->context, { preferred, fallback });
    REQUIRE(handler);

    const auto aabb = handler->getAABB(this->context, { preferred, fallback });
    REQUIRE(aabb.left == Catch::Approx(-1.0f));
    REQUIRE(aabb.right == Catch::Approx(7.0f));
    REQUIRE(aabb.top == Catch::Approx(0.0f));
    REQUIRE(aabb.bottom == Catch::Approx(8.0f));
}
