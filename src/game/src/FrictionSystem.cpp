#include "../include/FrictionSystem/FrictionSystem.h"

#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

void FrictionSystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GroundedComponent>(ctx.world.components());
    
    for (auto [entity, v, g] : view)
    {
        if (ctx.world.components().has<HitstopComponent>(entity))
        { if (ctx.world.components().get<HitstopComponent>(entity).frozen) continue; }

        if (!g.onGround) continue;

        float effectiveFriction = this->friction * (1.0f - g.frictionReduction);
        if (effectiveFriction < 0.0f) effectiveFriction = 0.0f;

        float decay = 1.0f - (effectiveFriction * 0.01f * ctx.deltaTime);
        if (decay < 0.0f) decay = 0.0f;

        v.vx *= decay;

        if (std::abs(v.vx) < 1.0f) v.vx = 0.0f;
    }
}
