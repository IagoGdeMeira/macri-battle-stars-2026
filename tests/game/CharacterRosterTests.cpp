#include "../../src/game/include/CharacterRoster/CharacterRoster.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("CharacterRoster stores and finds entries by id", "[unit][character_roster]")
{
    CharacterRoster roster;
    roster.addEntry({"fighter_01", "Ryu", "assets/characters/fighter_01.json"});
    roster.addEntry({"fighter_02", "Ken", "assets/characters/fighter_02.json"});

    REQUIRE(roster.getAll().size() == 2);

    const auto* ryu = roster.findById("fighter_01");
    REQUIRE(ryu != nullptr);
    REQUIRE(ryu->name == "Ryu");
    REQUIRE(ryu->definitionPath == "assets/characters/fighter_01.json");

    REQUIRE(roster.findById("missing") == nullptr);
}
