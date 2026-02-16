#include "../../src/domain/include/ComponentStorage/ComponentStorage.h"

#include <catch2/catch_test_macros.hpp>

struct Position { float x, y; };

struct ComponentStorageFixture { ComponentStorage<Position> storage; };

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage adds and retrieves component",
    "[unit][component_storage]")
{
    Entity e{0, 0};
    storage.add(e, {10, 20});

    REQUIRE(storage.has(e));
    REQUIRE(storage.size() == 1);

    Position &p = storage.get(e);
    REQUIRE(p.x == 10);
    REQUIRE(p.y == 20);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage get throws if component not found",
    "[unit][component_storage]")
{
    Entity e{1, 0};

    REQUIRE_FALSE(storage.has(e));
    REQUIRE_THROWS(storage.get(e));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage throws when adding duplicate component",
    "[unit][component_storage]")
{
    Entity e{2, 0};

    storage.add(e, {1, 1});

    REQUIRE_THROWS(storage.add(e, {2, 2}));
    REQUIRE(storage.size() == 1);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage removes component correctly",
    "[unit][component_storage]")
{
    Entity e{3, 0};

    storage.add(e, {5, 6});
    REQUIRE(storage.size() == 1);

    storage.remove(e);

    REQUIRE(storage.size() == 0);
    REQUIRE_FALSE(storage.has(e));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "Removing non-existent component does nothing",
    "[unit][component_storage]")
{
    Entity e{4, 0};

    storage.remove(e);

    REQUIRE(storage.size() == 0);
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage keeps dense array compact after removal",
    "[unit][component_storage]")
{
    Entity e1{0, 0};
    Entity e2{1, 0};
    Entity e3{2, 0};

    storage.add(e1, {1, 1});
    storage.add(e2, {2, 2});
    storage.add(e3, {3, 3});

    REQUIRE(storage.size() == 3);

    storage.remove(e2);

    REQUIRE(storage.size() == 2);

    REQUIRE(storage.has(e1));
    REQUIRE_FALSE(storage.has(e2));
    REQUIRE(storage.has(e3));

    REQUIRE(storage.getEntities().size() == storage.getComponents().size());
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage distinguishes entities by generation",
    "[unit][component_storage]")
{
    Entity e1{5, 0};
    Entity e2{5, 1};

    storage.add(e1, {9, 9});

    REQUIRE(storage.has(e1));
    REQUIRE_FALSE(storage.has(e2));
}

TEST_CASE_METHOD(
    ComponentStorageFixture,
    "ComponentStorage allows modifying component through get",
    "[unit][component_storage]")
{
    Entity e{6, 0};

    storage.add(e, {7, 7});
    storage.get(e).x = 42;

    REQUIRE(storage.get(e).x == 42);
}
