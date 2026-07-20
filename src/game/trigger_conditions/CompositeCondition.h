#ifndef composite_condition_h
#define composite_condition_h

#include "ITriggerCondition/ITriggerCondition.h"

#include <memory>
#include <vector>

class CompositeCondition : public ITriggerCondition
{
public:
    enum Type { And, Or };

    CompositeCondition(Type type, std::vector<std::unique_ptr<ITriggerCondition>> conditions) :
        type(type), conditions(std::move(conditions)) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        switch (this->type)
        {
            case Type::And:
                for (const auto& cond : this->conditions) if (!cond->evaluate(ctx)) return false;
                return true;
            case Type::Or:
                for (const auto& cond : this->conditions) if (cond->evaluate(ctx)) return true;
                return false;
        }
        return false;
    }

private:
    Type type;
    std::vector<std::unique_ptr<ITriggerCondition>> conditions;
};

#endif // composite_condition_h
