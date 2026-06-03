#include "../../src/game/include/MapRoster/MapRoster.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MapRoster stores and finds entries by id", "[unit][map_roster]")
{
    MapRoster roster;
    roster.addEntry({"dojo", "Dojo", "assets/maps/dojo.json"});
    roster.addEntry({"roof", "Rooftop", "assets/maps/roof.json"});

    REQUIRE(roster.getAll().size() == 2);

    const auto* dojo = roster.findById("dojo");
    REQUIRE(dojo != nullptr);
    REQUIRE(dojo->name == "Dojo");
    REQUIRE(dojo->definitionPath == "assets/maps/dojo.json");

    REQUIRE(roster.findById("missing") == nullptr);
}
