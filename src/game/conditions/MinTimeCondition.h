#ifndef min_time_condition_h
#define min_time_condition_h

#include "../include/ICondition/ICondition.h"

class MinTimeCondition : public ICondition
{
public:
    MinTimeCondition(float time) : time(time) {}

    bool evaluate(const ConditionContext& ctx) const override
    { return ctx.stateComponent.timeInState >= this->time; }

private:
    float time;
};

#endif // min_time_condition_h
