#include "../../src/domain/include/ComponentType/ComponentType.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "ComponentType returns same id for same type",
    "[unit][component_type]"
) {
    struct LocalPosition {};

    size_t id1 = ComponentType::id<LocalPosition>();
    size_t id2 = ComponentType::id<LocalPosition>();

    REQUIRE(id1 == id2);
}

TEST_CASE(
    "ComponentType returns different ids for different types",
    "[unit][component_type]"
) {
    struct LocalPosition {};
    struct LocalVelocity {};

    size_t posId = ComponentType::id<LocalPosition>();
    size_t velId = ComponentType::id<LocalVelocity>();

    REQUIRE(posId != velId);
}

TEST_CASE(
    "ComponentType id remains stable across interleaved calls",
    "[unit][component_type]"
) {
    struct LocalPosition {};
    struct LocalVelocity {};

    size_t pos1 = ComponentType::id<LocalPosition>();
    size_t vel1 = ComponentType::id<LocalVelocity>();
    size_t pos2 = ComponentType::id<LocalPosition>();

    REQUIRE(pos1 == pos2);
    REQUIRE(pos1 != vel1);
}

TEST_CASE(
    "ComponentType assigns incrementing ids",
    "[unit][component_type]"
) {
    struct LocalA {};
    struct LocalB {};
    struct LocalC {};

    size_t id1 = ComponentType::id<LocalA>();
    size_t id2 = ComponentType::id<LocalB>();
    size_t id3 = ComponentType::id<LocalC>();

    REQUIRE(id2 == id1 + 1);
    REQUIRE(id3 == id2 + 1);
}