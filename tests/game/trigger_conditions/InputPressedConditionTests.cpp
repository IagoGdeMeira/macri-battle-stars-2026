#include "game/trigger_conditions/InputPressedCondition.h"

#include "domain/components/InputComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>

class InputPressedConditionFixture
{
public:
    World world;
    StateComponent state;
    Entity entity{1};

    InputPressedConditionFixture()
    { this->world.components().registerComponent<InputComponent>(); }

    TriggerConditionContext makeContext()
    { return TriggerConditionContext { this->world, this->entity, this->state }; }

    void addInputComponent(InputAction action, bool pressed = false)
    {
        InputComponent input;
        input.actions[action] = InputComponent::State { pressed, 0.f };
        this->world.components().add<InputComponent>(this->entity, input);
    }
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
