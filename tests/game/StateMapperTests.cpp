#include "../../src/game/include/StateMapper/StateMapper.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

TEST_CASE("StateMapper maps known state names",
    "[unit][state_mapper]"
) {
    REQUIRE(StateMapper::fromString("Idle") == StateId::Idle);
    REQUIRE(StateMapper::fromString("Punching") == StateId::Punching);
    REQUIRE(StateMapper::fromString("Kicking") == StateId::Kicking);
}

TEST_CASE("StateMapper rejects unknown state names",
    "[unit][state_mapper]"
) {
    REQUIRE_THROWS_AS(StateMapper::fromString("Flying"), std::invalid_argument);
}
