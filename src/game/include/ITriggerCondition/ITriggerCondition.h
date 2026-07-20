#ifndef i_trigger_condition_h
#define i_trigger_condition_h

#include "TriggerConditionContext/TriggerConditionContext.h"

class ITriggerCondition
{
public:
    virtual ~ITriggerCondition() = default;
    virtual bool evaluate(const TriggerConditionContext& ctx) const = 0;
};

#endif // i_trigger_condition_h
