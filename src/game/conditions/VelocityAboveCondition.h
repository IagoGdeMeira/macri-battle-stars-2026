#ifndef velocity_above_condition_h
#define velocity_above_condition_h

#include "../include/ICondition/ICondition.h"

#include "../../domain/components/VelocityComponent.h"

#include <cmath>

class VelocityAboveCondition : public ICondition
{
public:
    VelocityAboveCondition(float threshold) : threshold(threshold) {}

    bool evaluate(const ConditionContext& ctx) const override
    {
        auto& components = ctx.world.components();

        if (!components.has<VelocityComponent>(ctx.entity)) return false;

        const auto& vel = components.get<VelocityComponent>(ctx.entity);
        float speed = std::sqrt(vel.vx * vel.vx + vel.vy * vel.vy);

        return speed >= this->threshold;
    }

private:
    float threshold;
};

#endif // velocity_above_condition_h
