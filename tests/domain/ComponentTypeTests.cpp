#include "../../src/domain/include/ComponentType/ComponentType.h"

#include <catch2/catch_test_macros.hpp>

struct Position {};
struct Velocity {};
struct Health {};
struct Special {};

TEST_CASE(
    "ComponentType returns same id for same type",
    "[unit][component_type]")
{
    size_t id1 = ComponentType::id<Position>();
    size_t id2 = ComponentType::id<Position>();

    REQUIRE(id1 == id2);
}

TEST_CASE(
    "ComponentType returns different ids for different types",
    "[unit][component_type]")
{
    size_t posId = ComponentType::id<Position>();
    size_t velId = ComponentType::id<Velocity>();

    REQUIRE(posId != velId);
}

TEST_CASE(
    "ComponentType id remains stable across interleaved calls",
    "[unit][component_type]")
{
    size_t pos1 = ComponentType::id<Position>();
    size_t vel1 = ComponentType::id<Velocity>();
    size_t pos2 = ComponentType::id<Position>();

    REQUIRE(pos1 == pos2);
    REQUIRE(pos1 != vel1);
}

TEST_CASE(
    "ComponentType assigns incrementing ids",
    "[unit][component_type]"
) {
    size_t id1 = ComponentType::id<Health>();
    size_t id2 = ComponentType::id<Special>();

    REQUIRE(id2 == id1 + 1);
}
