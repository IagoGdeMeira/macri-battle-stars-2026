#ifndef composite_condition_h
#define composite_condition_h

#include "../include/ICondition/ICondition.h"

#include <memory>
#include <vector>

class CompositeCondition : public ICondition
{
public:
    enum CompositeType { And, Or };

    CompositeCondition(CompositeType type, std::vector<std::unique_ptr<ICondition>> conditions) :
        type(type), conditions(std::move(conditions)) {}

    bool evaluate(const ConditionContext& ctx) const override
    {
        switch (this->type)
        {
        case CompositeType::And:
            for (const auto& cond : this->conditions) if (!cond->evaluate(ctx)) return false;
            return true;
        case CompositeType::Or:
            for (const auto& cond : this->conditions) if (cond->evaluate(ctx)) return true;
            return false;
        }
        return false;
    }

private:
    CompositeType type;
    std::vector<std::unique_ptr<ICondition>> conditions;
};

#endif // composite_condition_h
