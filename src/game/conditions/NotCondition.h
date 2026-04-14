#ifndef not_condition_h
#define not_condition_h

#include "../include/ICondition/ICondition.h"

#include <memory>

class NotCondition : public ICondition
{
public:
    NotCondition(std::unique_ptr<ICondition> condition) : condition(std::move(condition)) {}

    bool evaluate(const ConditionContext& ctx) const override
    { return !this->condition->evaluate(ctx); }

private:
    std::unique_ptr<ICondition> condition;
};

#endif // not_condition_h
