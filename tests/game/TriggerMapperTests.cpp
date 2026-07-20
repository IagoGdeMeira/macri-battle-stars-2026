#include "game/include/TriggerMapper/TriggerMapper.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

TEST_CASE("TriggerMapper maps known trigger names", "[unit][trigger_mapper]")
{
    REQUIRE(TriggerMapper::fromString("Punched") == TriggerId::Punched);
    REQUIRE(TriggerMapper::fromString("Kicked") == TriggerId::Kicked);
    REQUIRE(TriggerMapper::fromString("Landed") == TriggerId::Landed);
}

TEST_CASE("TriggerMapper rejects unknown trigger names", "[unit][trigger_mapper]")
{ REQUIRE_THROWS_AS(TriggerMapper::fromString("Nothing"), std::invalid_argument); }
