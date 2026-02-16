#include "../../src/domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

struct Position { int x, y; };
struct Velocity { int dx, dy; };
struct Health { int hp; };

struct ComponentManagerFixture { ComponentManager manager; };

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager creates storage on first add",
    "[unit][component_manager]")
{
    Entity e{0, 0};

    this->manager.add<Position>(e, {10, 20});

    REQUIRE(this->manager.has<Position>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager has returns false if storage does not exist",
    "[unit][component_manager]")
{
    Entity e{1, 0};

    REQUIRE_FALSE(this->manager.has<Velocity>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager get throws if storage does not exist",
    "[unit][component_manager]")
{
    Entity e{2, 0};

    REQUIRE_THROWS(this->manager.get<Position>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager get throws if entity does not have component",
    "[unit][component_manager]")
{
    Entity e{3, 0};

    this->manager.add<Position>(e, {1, 1});

    Entity other{4, 0};

    REQUIRE_THROWS(this->manager.get<Position>(other));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Different component types are stored independently",
    "[unit][component_manager]")
{
    Entity e{5, 0};

    this->manager.add<Position>(e, {1, 1});
    this->manager.add<Velocity>(e, {2, 2});

    REQUIRE(this->manager.has<Position>(e));
    REQUIRE(this->manager.has<Velocity>(e));

    REQUIRE(this->manager.get<Position>(e).x == 1);
    REQUIRE(this->manager.get<Velocity>(e).dx == 2);
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Removing one component type does not affect others",
    "[unit][component_manager]")
{
    Entity e{6, 0};

    this->manager.add<Position>(e, {5, 5});
    this->manager.add<Velocity>(e, {6, 6});

    this->manager.remove<Position>(e);

    REQUIRE_FALSE(this->manager.has<Position>(e));
    REQUIRE(this->manager.has<Velocity>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Removing component from non-existing storage does nothing",
    "[unit][component_manager]")
{
    Entity e{7, 0};

    this->manager.remove<Health>(e);

    REQUIRE_FALSE(this->manager.has<Health>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "removeAll removes entity from all component storages",
    "[unit][component_manager]")
{
    Entity e{8, 0};

    this->manager.add<Position>(e, {1, 1});
    this->manager.add<Velocity>(e, {2, 2});
    this->manager.add<Health>(e, {100});

    REQUIRE(this->manager.has<Position>(e));
    REQUIRE(this->manager.has<Velocity>(e));
    REQUIRE(this->manager.has<Health>(e));

    this->manager.removeAll(e);

    REQUIRE_FALSE(this->manager.has<Position>(e));
    REQUIRE_FALSE(this->manager.has<Velocity>(e));
    REQUIRE_FALSE(this->manager.has<Health>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "removeAll affects only specified entity",
    "[unit][component_manager]")
{
    Entity e1{9, 0};
    Entity e2{10, 0};

    this->manager.add<Position>(e1, {1, 1});
    this->manager.add<Position>(e2, {2, 2});

    this->manager.removeAll(e1);

    REQUIRE_FALSE(this->manager.has<Position>(e1));
    REQUIRE(this->manager.has<Position>(e2));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "findStorage returns nullptr before creation and valid after",
    "[unit][component_manager]")
{
    REQUIRE(this->manager.findStorage<Velocity>() == nullptr);

    Entity e{11, 0};
    this->manager.add<Velocity>(e, {3, 3});

    REQUIRE(this->manager.findStorage<Velocity>() != nullptr);
}
