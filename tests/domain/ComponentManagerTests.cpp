#include "../../src/domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

struct ComponentManagerFixture
{
    ComponentManager manager;
};

struct Position { int x, y; };

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager stores component",
    "[unit][component_manager]")
{
    Entity e = {0, 0};
    Position p{42, 84};

    manager.add<Position>(e, p);

    REQUIRE(manager.has<Position>(e));
    REQUIRE(manager.get<Position>(e).x == 42);
    REQUIRE(manager.get<Position>(e).y == 84);
}

TEST_CASE_METHOD(
    ComponentManagerFixture,
    "ComponentManager::removeAll removes components from entity",
    "[unit][component_manager]")
{
    Entity e = {0, 0};
    Position p{10};

    manager.add<Position>(e, p);
    REQUIRE(manager.has<Position>(e));

    manager.removeAll(e);
    REQUIRE_FALSE(manager.has<Position>(e));
}
