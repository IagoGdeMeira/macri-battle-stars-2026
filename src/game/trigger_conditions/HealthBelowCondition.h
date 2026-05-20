#ifndef health_below_condition_h
#define health_below_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

#include "../../domain/components/HealthComponent.h"

class HealthBelowCondition : public ITriggerCondition
{
public:
    HealthBelowCondition(int threshold) : threshold(threshold) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& components = ctx.world.components();

        if (!components.has<HealthComponent>(ctx.entity)) return false;
        const auto& health = components.get<HealthComponent>(ctx.entity);

        return health.current <= this->threshold;
    }

private:
    int threshold;
};

#endif // health_below_condition_h
