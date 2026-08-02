#include "game/collision_handlers/CircleCollisionHandler.h"

#include "domain/components/CircleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

class CircleCollisionHandlerFixture
{
public:
    CircleCollisionHandlerFixture() : context { this->world, this->bus, this->commandBuffer, 0.f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<CircleColliderComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext context;
    CircleCollisionHandler handler;

    Entity createCircle(float x, float y, float radius)
    {
        const Entity entity = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, TransformComponent{x, y, 1.f, 1.f, 0.f});
        comp.add<CircleColliderComponent>(entity, CircleColliderComponent{radius});
        return entity;
    }
};

TEST_CASE_METHOD(CircleCollisionHandlerFixture,
    "CircleCollisionHandler::getAABB returns correct bounds for a circle",
    "[unit][circle_collision_handler]"
) {
    const Entity circle = this->createCircle(10.f, 20.f, 5.f);
    const AABB aabb = this->handler.getAABB(this->context, { circle, std::nullopt });

    REQUIRE(aabb.left == Catch::Approx(5.f));
    REQUIRE(aabb.right == Catch::Approx(15.f));
    REQUIRE(aabb.top == Catch::Approx(15.f));
    REQUIRE(aabb.bottom == Catch::Approx(25.f));
}

TEST_CASE_METHOD(CircleCollisionHandlerFixture,
    "CircleCollisionHandler::getAABB falls back to the secondary entity when preferred has no collider",
    "[unit][circle_collision_handler]"
) {
    const Entity preferred = this->world.entities().create();
    this->world.components().add<TransformComponent>(preferred, TransformComponent{0.f, 0.f});

    const Entity fallback = this->createCircle(5.f, 5.f, 3.f);

    const AABB aabb = this->handler.getAABB(this->context, { preferred, fallback });

    REQUIRE(aabb.left == Catch::Approx(-3.f));
    REQUIRE(aabb.right == Catch::Approx(3.f));
    REQUIRE(aabb.top == Catch::Approx(-3.f));
    REQUIRE(aabb.bottom == Catch::Approx(3.f));
}

TEST_CASE_METHOD(CircleCollisionHandlerFixture,
    "CircleCollisionHandler::getAABB throws when no entity has CircleColliderComponent",
    "[unit][circle_collision_handler]"
) {
    const Entity a = this->world.entities().create();
    const Entity b = this->world.entities().create();
    this->world.components().add<TransformComponent>(a, TransformComponent{});
    this->world.components().add<TransformComponent>(b, TransformComponent{});

    REQUIRE_THROWS_AS(this->handler.getAABB(this->context, { a, b }), std::runtime_error);
}

TEST_CASE_METHOD(CircleCollisionHandlerFixture,
    "CircleCollisionHandler::getTransform returns the transform of the preferred entity",
    "[unit][circle_collision_handler]"
) {
    const Entity circle = this->createCircle(7.f, 9.f, 4.f);
    auto& transform = this->handler.getTransform(this->context, { circle, std::nullopt });

    REQUIRE(transform.position.x == Catch::Approx(7.f));
    REQUIRE(transform.position.y == Catch::Approx(9.f));
}

TEST_CASE_METHOD(CircleCollisionHandlerFixture, "CircleCollisionHandler::getTransform falls back to the secondary entity",
    "[unit][circle_collision_handler]"
) {
    const Entity preferred = this->world.entities().create();
    const Entity fallback = this->createCircle(3.f, 4.f, 2.f);

    auto& transform = this->handler.getTransform(this->context, { preferred, fallback });
    REQUIRE(transform.position.x == Catch::Approx(3.f));
    REQUIRE(transform.position.y == Catch::Approx(4.f));
}

TEST_CASE_METHOD(CircleCollisionHandlerFixture,
    "CircleCollisionHandler::getTransform throws when no entity has TransformComponent",
    "[unit][circle_collision_handler]"
) {
    const Entity a = this->world.entities().create();
    const Entity b = this->world.entities().create();

    REQUIRE_THROWS_AS(this->handler.getTransform(this->context, { a, b }), std::runtime_error);
}
