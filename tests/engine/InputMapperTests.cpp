#include "../src/engine/include/InputMapper/InputMapper.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("InputMapper converts known and unknown action strings",
    "[unit][input_mapper]"
) {
    REQUIRE(InputMapper::stringToAction("MoveUp") == InputAction::MoveUp);
    REQUIRE(InputMapper::stringToAction("Punch") == InputAction::Punch);
    REQUIRE(InputMapper::stringToAction("Kick") == InputAction::Kick);
    REQUIRE(InputMapper::stringToAction("invalid") == InputAction::None);
}

TEST_CASE("InputMapper converts known and unknown key strings",
    "[unit][input_mapper]"
) {
    REQUIRE(InputMapper::stringToKey("A") == KeyCode::A);
    REQUIRE(InputMapper::stringToKey("Escape") == KeyCode::Escape);
    REQUIRE(InputMapper::stringToKey("Numpad9") == KeyCode::Numpad9);
    REQUIRE(InputMapper::stringToKey("invalid") == KeyCode::Unknown);
}

TEST_CASE("InputMapper converts enums to strings with fallback",
    "[unit][input_mapper]"
) {
    REQUIRE(InputMapper::actionToString(InputAction::Defend) == "Defend");
    REQUIRE(InputMapper::actionToString(InputAction::Punch) == "Punch");
    REQUIRE(InputMapper::actionToString(InputAction::Kick) == "Kick");
    REQUIRE(InputMapper::actionToString(InputAction::None) == "None");

    REQUIRE(InputMapper::keyToString(KeyCode::F12) == "F12");
    REQUIRE(InputMapper::keyToString(KeyCode::Unknown) == "Unknown");
}

TEST_CASE("InputMapper supports round-trip conversion for representative values",
    "[unit][input_mapper]"
) {
    const auto action = InputAction::Punch;
    REQUIRE(InputMapper::stringToAction(InputMapper::actionToString(action)) == action);

    const auto kick = InputAction::Kick;
    REQUIRE(InputMapper::stringToAction(InputMapper::actionToString(kick)) == kick);

    const auto key = KeyCode::Left;
    REQUIRE(InputMapper::stringToKey(InputMapper::keyToString(key)) == key);
}
