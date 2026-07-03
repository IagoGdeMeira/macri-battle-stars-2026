#ifndef input_pressed_condition_h
#define input_pressed_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

#include "../../domain/components/InputComponent.h"

class InputPressedCondition : public ITriggerCondition
{
public:
    InputPressedCondition(InputAction action) : action(action) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& components = ctx.world.components();

        if (!components.has<InputComponent>(ctx.entity)) return false;

        const auto& input = components.get<InputComponent>(ctx.entity);
        const auto it = input.actions.find(this->action);
        return it != input.actions.end() && it->second.pressed;
    }

private:
    InputAction action;
};

#endif // input_pressed_condition_h
