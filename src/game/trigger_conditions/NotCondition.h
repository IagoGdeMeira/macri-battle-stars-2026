#ifndef not_condition_h
#define not_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

#include <memory>

class NotCondition : public ITriggerCondition
{
public:
    NotCondition(std::unique_ptr<ITriggerCondition> condition) : condition(std::move(condition)) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    { return !this->condition->evaluate(ctx); }

private:
    std::unique_ptr<ITriggerCondition> condition;
};

#endif // not_condition_h
