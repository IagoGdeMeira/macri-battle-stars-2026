#ifndef vertical_velocity_above_condition_h
#define vertical_velocity_above_condition_h

#include "ITriggerCondition/ITriggerCondition.h"

#include "domain/components/VelocityComponent.h"

class VerticalVelocityAboveCondition : public ITriggerCondition
{
public:
    VerticalVelocityAboveCondition(float threshold) : threshold(threshold) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& comp = ctx.world.components();
        if (!comp.has<VelocityComponent>(ctx.entity)) return false;
        const auto& vel = comp.get<VelocityComponent>(ctx.entity).velocity;
        return vel.y > this->threshold;
    }

private:
    float threshold;
};

#endif // vertical_velocity_above_condition_h
