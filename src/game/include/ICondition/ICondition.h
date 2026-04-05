#ifndef i_condition_h
#define i_condition_h

#include "../ConditionContext/ConditionContext.h"

class ICondition
{
public:
    virtual ~ICondition() = default;

    virtual bool evaluate(const ConditionContext& ctx) const = 0;
};

#endif // i_condition_h
