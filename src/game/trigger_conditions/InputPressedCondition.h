#ifndef input_pressed_condition_h
#define input_pressed_condition_h

#include "ITriggerCondition/ITriggerCondition.h"

#include "domain/components/InputComponent.h"

class InputPressedCondition : public ITriggerCondition
{
public:
    InputPressedCondition(InputAction action) : action(action) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& comp = ctx.world.components();

        if (!comp.has<InputComponent>(ctx.entity)) return false;

        const auto& input = comp.get<InputComponent>(ctx.entity);
        const auto it = input.actions.find(this->action);
        return it != input.actions.end() && it->second.pressed;
    }

private:
    InputAction action;
};

#endif // input_pressed_condition_h
