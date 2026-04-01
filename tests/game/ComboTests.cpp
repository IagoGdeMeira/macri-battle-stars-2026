#include "../../src/game/include/Combo/Combo.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Combo uses expected defaults",
    "[unit][combo]"
) {
    Combo combo;

    REQUIRE(combo.name.empty());
    REQUIRE(combo.steps.empty());
    REQUIRE(combo.priority == 0);
    REQUIRE(combo.consumeInput == true);
}

TEST_CASE("ComboStep stores action and maxDelay",
    "[unit][combo]"
) {
    ComboStep step { InputAction::Attack, 120 };

    REQUIRE(step.action == InputAction::Attack);
    REQUIRE(step.maxDelay == 120);
}
