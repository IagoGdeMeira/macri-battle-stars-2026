#include "../../src/game/include/StateMapper/StateMapper.h"

#include "../../src/game/include/StateIdMapper/StateIdMapper.h"

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
) { REQUIRE_THROWS_AS(StateMapper::fromString("Flying"), std::invalid_argument); }

TEST_CASE("StateIdMapper resolves base and custom states per instance",
    "[unit][state_mapper]"
) {
    StateIdMapper mapperA;
    StateIdMapper mapperB;

    const auto baseIdle = mapperA.fromString("Idle");
    REQUIRE(baseIdle == StateId::Idle);

    const auto unknownBefore = mapperA.fromString("PowerCharge");
    REQUIRE(unknownBefore == StateId::Unknown);

    const auto customA = mapperA.addCustomMapping("PowerCharge");
    REQUIRE(customA.isCustom());
    REQUIRE(mapperA.fromString("PowerCharge") == customA);
    REQUIRE(mapperA.toString(customA) == "PowerCharge");

    REQUIRE(mapperB.fromString("PowerCharge") == StateId::Unknown);
}
