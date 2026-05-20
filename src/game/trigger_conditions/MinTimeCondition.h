#ifndef min_time_condition_h
#define min_time_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

class MinTimeCondition : public ITriggerCondition
{
public:
    MinTimeCondition(float time) : time(time) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    { return ctx.stateComponent.timeInState >= this->time; }

private:
    float time;
};

#endif // min_time_condition_h
