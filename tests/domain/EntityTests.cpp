#include "../../src/domain/include/Entity/Entity.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Same entity IDs reference the same entity", "[unit][entity]")
{
    Entity e1(0);
    Entity e2(0);

    REQUIRE(e1 == e2);
}

TEST_CASE("Different entity IDs reference different entities", "[unit][entity]")
{
    Entity e1(0);
    Entity e2(1);

    REQUIRE(e1 != e2);
}
