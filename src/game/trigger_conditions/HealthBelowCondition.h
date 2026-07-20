#ifndef health_below_condition_h
#define health_below_condition_h

#include "ITriggerCondition/ITriggerCondition.h"

#include "domain/components/HealthComponent.h"

class HealthBelowCondition : public ITriggerCondition
{
public:
    HealthBelowCondition(int threshold) : threshold(threshold) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& comp = ctx.world.components();

        if (!comp.has<HealthComponent>(ctx.entity)) return false;
        const auto& health = comp.get<HealthComponent>(ctx.entity);

        return health.current <= this->threshold;
    }

private:
    int threshold;
};

#endif // health_below_condition_h
