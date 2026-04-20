#include "../include/FrictionSystem/FrictionSystem.h"

#include "../../domain/components/VelocityComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

FrictionSystem::FrictionSystem(float friction) : friction(friction) {}

void FrictionSystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GroundedComponent>(ctx.world.components());
    
    for (auto [e, v, g] : view)
    {
        if (!g.onGround) continue;

        float effectiveFriction = this->friction * (1.0f - g.frictionReduction);
        if (effectiveFriction < 0.0f) effectiveFriction = 0.0f;

        float decay = 1.0f - (effectiveFriction * 0.01f * ctx.deltaTime);
        if (decay < 0.0f) decay = 0.0f;

        v.vx *= decay;

        if (std::abs(v.vx) < 1.0f) v.vx = 0.0f;
    }
}