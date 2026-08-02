#include "game/collision_handlers/RectangleCollisionHandler.h"

#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

class RectangleCollisionHandlerFixture
{
public:
    RectangleCollisionHandlerFixture() : context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RectangleColliderComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext context;
    RectangleCollisionHandler handler;

    Entity createRect(float x, float y, float width, float height)
    {
        const Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<RectangleColliderComponent>(entity, RectangleColliderComponent{width, height});
        return entity;
    }
};

TEST_CASE_METHOD(RectangleCollisionHandlerFixture, "RectangleCollisionHandler::getAABB returns correct AABB for a rectangle",
    "[unit][rectangle_collision_handler]"
) {
    const Entity rect = this->createRect(10.f, 20.f, 6.f, 8.f);
    const AABB aabb = this->handler.getAABB(this->context, { rect, std::nullopt });

    REQUIRE(aabb.left == Catch::Approx(7.f));
    REQUIRE(aabb.right == Catch::Approx(13.f));
    REQUIRE(aabb.top == Catch::Approx(16.f));
    REQUIRE(aabb.bottom == Catch::Approx(24.f));
}

TEST_CASE_METHOD(RectangleCollisionHandlerFixture,
    "RectangleCollisionHandler::getAABB falls back to the secondary entity when preferred has no collider",
    "[unit][rectangle_collision_handler]"
) {
    const Entity preferred = this->world.entities().create();
    this->world.components().add<TransformComponent>(preferred, TransformComponent{0.f, 0.f});

    const Entity fallback = this->createRect(5.f, 5.f, 4.f, 4.f);

    const AABB aabb = this->handler.getAABB(this->context, { preferred, fallback });

    REQUIRE(aabb.left == Catch::Approx(-2.f));
    REQUIRE(aabb.right == Catch::Approx(2.f));
    REQUIRE(aabb.top == Catch::Approx(-2.f));
    REQUIRE(aabb.bottom == Catch::Approx(2.f));
}

TEST_CASE_METHOD(RectangleCollisionHandlerFixture,
    "RectangleCollisionHandler::getAABB throws when no entity has RectangleColliderComponent",
    "[unit][rectangle_collision_handler]"
) {
    const Entity a = this->world.entities().create();
    const Entity b = this->world.entities().create();
    this->world.components().add<TransformComponent>(a, TransformComponent{});
    this->world.components().add<TransformComponent>(b, TransformComponent{});

    REQUIRE_THROWS_AS(this->handler.getAABB(this->context, { a, b }), std::runtime_error);
}

TEST_CASE_METHOD(RectangleCollisionHandlerFixture,
    "RectangleCollisionHandler::getTransform returns the transform of the preferred entity",
    "[unit][rectangle_collision_handler]"
) {
    const Entity rect = this->createRect(7.f, 9.f, 4.f, 6.f);
    auto& transform = this->handler.getTransform(this->context, { rect, std::nullopt });

    REQUIRE(transform.position.x == Catch::Approx(7.f));
    REQUIRE(transform.position.y == Catch::Approx(9.f));
}

TEST_CASE_METHOD(RectangleCollisionHandlerFixture, "RectangleCollisionHandler::getTransform falls back to the secondary entity",
    "[unit][rectangle_collision_handler]"
) {
    const Entity preferred = this->world.entities().create();
    const Entity fallback = this->createRect(3.f, 4.f, 2.f, 2.f);

    auto& transform = this->handler.getTransform(this->context, { preferred, fallback });
    REQUIRE(transform.position.x == Catch::Approx(3.f));
    REQUIRE(transform.position.y == Catch::Approx(4.f));
}

TEST_CASE_METHOD(RectangleCollisionHandlerFixture,
    "RectangleCollisionHandler::getTransform throws when no entity has TransformComponent",
    "[unit][rectangle_collision_handler]"
) {
    const Entity a = this->world.entities().create();
    const Entity b = this->world.entities().create();

    REQUIRE_THROWS_AS(this->handler.getTransform(this->context, { a, b }), std::runtime_error);
}
