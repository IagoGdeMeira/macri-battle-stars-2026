#ifndef input_pressed_condition_h
#define input_pressed_condition_h

#include "../include/ICondition/ICondition.h"

#include "../../domain/components/InputComponent.h"

class InputPressedCondition : public ICondition
{
public:
    InputPressedCondition(InputAction action) : action(action) {}

    bool evaluate(const ConditionContext& ctx) const override
    {
        auto& components = ctx.world.components();

        if (!components.has<InputComponent>(ctx.entity)) return false;

        const auto& input = components.get<InputComponent>(ctx.entity);
        return input.actions.find(action) != input.actions.end();
    }

private:
    InputAction action;
};

#endif // input_pressed_condition_h
