#ifndef composite_condition_h
#define composite_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

#include <memory>
#include <vector>

class CompositeCondition : public ITriggerCondition
{
public:
    enum CompositeType { And, Or };

    CompositeCondition(CompositeType type, std::vector<std::unique_ptr<ITriggerCondition>> conditions) :
        type(type), conditions(std::move(conditions)) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
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
    std::vector<std::unique_ptr<ITriggerCondition>> conditions;
};

#endif // composite_condition_h
