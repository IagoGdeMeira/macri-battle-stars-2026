#include "../../../src/game/trigger_conditions/InputPressedCondition.h"

#include "../../../src/domain/components/InputComponent.h"
#include "../../../src/domain/components/StateComponent.h"
#include "../../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class InputPressedConditionFixture
{
public:
    InputPressedConditionFixture()
    { this->world.components().registerComponent<InputComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addInputComponent(InputAction action, bool pressed = false)
    {
        InputComponent input;
        input.actions[action] = InputState { pressed, 0.0f };
        this->world.components().add<InputComponent>(this->entity, input);
    }

    World world;
    StateComponent state;
    Entity entity { 1 };
};

TEST_CASE_METHOD(InputPressedConditionFixture, "InputPressedCondition returns true when action is pressed",
    "[unit][input_pressed_condition]"
) {
    this->addInputComponent(InputAction::Punch, true);

    REQUIRE(InputPressedCondition(InputAction::Punch).evaluate(this->makeContext()));
}

TEST_CASE_METHOD(InputPressedConditionFixture, "InputPressedCondition returns false when action is not pressed",
    "[unit][input_pressed_condition]"
) {
    this->addInputComponent(InputAction::Punch, false);

    REQUIRE_FALSE(InputPressedCondition(InputAction::Punch).evaluate(this->makeContext()));
}

TEST_CASE_METHOD(InputPressedConditionFixture, "InputPressedCondition returns false when action is not in component",
    "[unit][input_pressed_condition]"
) {
    this->addInputComponent(InputAction::Punch, true);

    REQUIRE_FALSE(InputPressedCondition(InputAction::Kick).evaluate(this->makeContext()));
}

TEST_CASE_METHOD(InputPressedConditionFixture, "InputPressedCondition fails without InputComponent",
    "[unit][input_pressed_condition]"
) { REQUIRE_FALSE(InputPressedCondition(InputAction::Punch).evaluate(this->makeContext())); }
