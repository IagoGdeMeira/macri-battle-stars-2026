#include "../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

struct WorldFixture
{
    World world;

    Entity createEntity() { return world.createEntity(); }

    template <typename Component>
    void add(Entity e, const Component &c) { world.addComponent<Component>(e, c); }
};

struct Position { int x, y; };

TEST_CASE_METHOD(
    WorldFixture,
    "World::createEntity returns distinct entities",
    "[unit][world]")
{
    Entity e1 = WorldFixture::createEntity();
    Entity e2 = WorldFixture::createEntity();

    REQUIRE(e1 != e2);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World::destroyEntity invalidates entity",
    "[unit][world]"
) {
    Entity e = WorldFixture::createEntity();
    world.destroyEntity(e);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World::addComponent stores and retrieves component",
    "[unit][world]"
) {
    Entity e = WorldFixture::createEntity();

    Position p{10, 20};
    WorldFixture::add(e, p);

    REQUIRE(world.hasComponent<Position>(e));

    Position &stored = world.getComponent<Position>(e);
    REQUIRE(stored.x == 10);
    REQUIRE(stored.y == 20);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World::removeComponent removes component",
    "[unit][world]")
{
    Entity e = WorldFixture::createEntity();

    Position p{1, 2};
    WorldFixture::add(e, p);

    REQUIRE(world.hasComponent<Position>(e));

    world.removeComponent<Position>(e);

    REQUIRE_FALSE(world.hasComponent<Position>(e));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World handles multiple entities independently",
    "[unit][world]")
{
    Entity e1 = WorldFixture::createEntity();
    Entity e2 = WorldFixture::createEntity();

    Position p1{5, 6};
    Position p2{7, 8};

    WorldFixture::add(e1, p1);
    WorldFixture::add(e2, p2);

    REQUIRE(world.getComponent<Position>(e1).x == 5);
    REQUIRE(world.getComponent<Position>(e2).x == 7);
}
