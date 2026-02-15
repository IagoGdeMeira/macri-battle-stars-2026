#include "../../src/domain/include/Entity/Entity.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "Entity exposes correct index and generation",
    "[unit][entity]")
{
    Entity e{42, 7};

    REQUIRE(e.index() == 42);
    REQUIRE(e.gen() == 7);
}

TEST_CASE(
    "Entities with same id and generation are equal",
    "[unit][entity]")
{
    Entity e1{3, 1};
    Entity e2{3, 1};

    REQUIRE(e1 == e2);
    REQUIRE_FALSE(e1 != e2);
}

TEST_CASE(
    "Entities with different id are not equal",
    "[unit][entity]")
{
    Entity e1{1, 0};
    Entity e2{2, 0};

    REQUIRE(e1 != e2);
    REQUIRE_FALSE(e1 == e2);
}

TEST_CASE(
    "Entities with different generation are not equal",
    "[unit][entity]")
{
    Entity e1{5, 0};
    Entity e2{5, 1};

    REQUIRE(e1 != e2);
    REQUIRE_FALSE(e1 == e2);
}

TEST_CASE(
    "Copied Entity preserves identity",
    "[unit][entity]")
{
    Entity original{8, 2};
    Entity copy = original;

    REQUIRE(copy == original);
    REQUIRE(copy.index() == 8);
    REQUIRE(copy.gen() == 2);
}

TEST_CASE(
    "Entity self comparison is always equal",
    "[unit][entity]")
{
    Entity e{10, 4};

    REQUIRE(e == e);
}
