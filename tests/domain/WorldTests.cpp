#include "../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y; };
struct Velocity { float dx, dy; };

struct WorldFixture { World world; };

TEST_CASE_METHOD(
    WorldFixture,
    "World creates and destroys entity correctly",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    REQUIRE(this->world.hasComponent<Position>(e) == false);

    this->world.destroyEntity(e);

    REQUIRE_FALSE(this->world.hasComponent<Position>(e));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World adds and retrieves component",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {10, 20});

    REQUIRE(this->world.hasComponent<Position>(e));

    Position &p = this->world.getComponent<Position>(e);
    REQUIRE(p.x == 10);
    REQUIRE(p.y == 20);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World throws when adding component to dead entity",
    "[unit][world]"
) {
    Entity e = this->world.createEntity();
    this->world.destroyEntity(e);

    REQUIRE_THROWS(this->world.addComponent<Position>(e, {1, 1}));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World throws when getting component from dead entity",
    "[unit][world]"
) {
    Entity e = this->world.createEntity();
    this->world.destroyEntity(e);

    REQUIRE_THROWS(this->world.getComponent<Position>(e));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World removes component correctly",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {5, 6});
    REQUIRE(this->world.hasComponent<Position>(e));

    this->world.removeComponent<Position>(e);

    REQUIRE_FALSE(this->world.hasComponent<Position>(e));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World destroyEntity removes all components",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {1, 1});
    this->world.addComponent<Velocity>(e, {2, 2});

    this->world.destroyEntity(e);

    REQUIRE_FALSE(this->world.hasComponent<Position>(e));
    REQUIRE_FALSE(this->world.hasComponent<Velocity>(e));
}

TEST_CASE_METHOD(
    WorldFixture,
    "World each iterates only matching entities",
    "[integration][world]"
) {
    Entity e1 = this->world.createEntity();
    Entity e2 = this->world.createEntity();
    Entity e3 = this->world.createEntity();

    this->world.addComponent<Position>(e1, {1, 1});
    this->world.addComponent<Velocity>(e1, {1, 1});

    this->world.addComponent<Position>(e2, {2, 2});

    this->world.addComponent<Position>(e3, {3, 3});
    this->world.addComponent<Velocity>(e3, {3, 3});

    int count = 0;

    this->world.each<Position, Velocity>([&](Entity, Position &, Velocity &){ count++; });

    REQUIRE(count == 2);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World each allows modifying components",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {1, 1});
    this->world.addComponent<Velocity>(e, {2, 2});

    this->world.each<Position, Velocity>([&](Entity, Position &p, Velocity &) { p.x = 42; });

    REQUIRE(this->world.getComponent<Position>(e).x == 42);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World each skips destroyed entities",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {1, 1});
    this->world.addComponent<Velocity>(e, {1, 1});

    this->world.destroyEntity(e);

    int count = 0;

    this->world.each<Position, Velocity>([&](Entity, Position &, Velocity &) { count++; });

    REQUIRE(count == 0);
}

TEST_CASE_METHOD(
    WorldFixture,
    "World each does nothing if one component type has no storage",
    "[integration][world]"
) {
    Entity e = this->world.createEntity();

    this->world.addComponent<Position>(e, {1, 1});
    int count = 0;

    this->world.each<Position, Velocity>([&](Entity, Position &, Velocity &) { count++; });

    REQUIRE(count == 0);
}
