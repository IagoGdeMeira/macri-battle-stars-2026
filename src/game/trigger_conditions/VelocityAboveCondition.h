#ifndef velocity_above_condition_h
#define velocity_above_condition_h

#include "../include/ITriggerCondition/ITriggerCondition.h"

#include "../../domain/components/VelocityComponent.h"

#include <cmath>

class VelocityAboveCondition : public ITriggerCondition
{
public:
    VelocityAboveCondition(float threshold) : threshold(threshold) {}

    bool evaluate(const TriggerConditionContext& ctx) const override
    {
        auto& components = ctx.world.components();

        if (!components.has<VelocityComponent>(ctx.entity)) return false;

        const auto& vel = components.get<VelocityComponent>(ctx.entity);
        float speed = std::sqrt(vel.velocity.x * vel.velocity.x + vel.velocity.y * vel.velocity.y);

        return speed >= this->threshold;
    }

private:
    float threshold;
};

#endif // velocity_above_condition_h
