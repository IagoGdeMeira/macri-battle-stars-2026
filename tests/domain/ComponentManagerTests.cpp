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

    manager.add<Position>(e, {10, 20});

    REQUIRE(manager.has<Position>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager has returns false if storage does not exist",
    "[unit][component_manager]")
{
    Entity e{1, 0};

    REQUIRE_FALSE(manager.has<Velocity>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager get throws if storage does not exist",
    "[unit][component_manager]")
{
    Entity e{2, 0};

    REQUIRE_THROWS(manager.get<Position>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager get throws if entity does not have component",
    "[unit][component_manager]")
{
    Entity e{3, 0};

    manager.add<Position>(e, {1, 1});

    Entity other{4, 0};

    REQUIRE_THROWS(manager.get<Position>(other));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Different component types are stored independently",
    "[unit][component_manager]")
{
    Entity e{5, 0};

    manager.add<Position>(e, {1, 1});
    manager.add<Velocity>(e, {2, 2});

    REQUIRE(manager.has<Position>(e));
    REQUIRE(manager.has<Velocity>(e));

    REQUIRE(manager.get<Position>(e).x == 1);
    REQUIRE(manager.get<Velocity>(e).dx == 2);
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Removing one component type does not affect others",
    "[unit][component_manager]")
{
    Entity e{6, 0};

    manager.add<Position>(e, {5, 5});
    manager.add<Velocity>(e, {6, 6});

    manager.remove<Position>(e);

    REQUIRE_FALSE(manager.has<Position>(e));
    REQUIRE(manager.has<Velocity>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "Removing component from non-existing storage does nothing",
    "[unit][component_manager]")
{
    Entity e{7, 0};

    manager.remove<Health>(e);

    REQUIRE_FALSE(manager.has<Health>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "removeAll removes entity from all component storages",
    "[unit][component_manager]")
{
    Entity e{8, 0};

    manager.add<Position>(e, {1, 1});
    manager.add<Velocity>(e, {2, 2});
    manager.add<Health>(e, {100});

    REQUIRE(manager.has<Position>(e));
    REQUIRE(manager.has<Velocity>(e));
    REQUIRE(manager.has<Health>(e));

    manager.removeAll(e);

    REQUIRE_FALSE(manager.has<Position>(e));
    REQUIRE_FALSE(manager.has<Velocity>(e));
    REQUIRE_FALSE(manager.has<Health>(e));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "removeAll affects only specified entity",
    "[unit][component_manager]")
{
    Entity e1{9, 0};
    Entity e2{10, 0};

    manager.add<Position>(e1, {1, 1});
    manager.add<Position>(e2, {2, 2});

    manager.removeAll(e1);

    REQUIRE_FALSE(manager.has<Position>(e1));
    REQUIRE(manager.has<Position>(e2));
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "findStorage returns nullptr before creation and valid after",
    "[unit][component_manager]")
{
    REQUIRE(manager.findStorage<Velocity>() == nullptr);

    Entity e{11, 0};
    manager.add<Velocity>(e, {3, 3});

    REQUIRE(manager.findStorage<Velocity>() != nullptr);
}
