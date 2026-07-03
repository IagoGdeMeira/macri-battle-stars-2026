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

        float effectiveFriction = this->friction * (1.f - g.frictionReduction);
        if (effectiveFriction < 0.f) effectiveFriction = 0.f;

        float decay = 1.f - (effectiveFriction * 0.01f * ctx.deltaTime);
        if (decay < 0.f) decay = 0.f;

        v.velocity.x *= decay;

        if (std::abs(v.velocity.x) < 1.f) v.velocity.x = 0.f;
    }
}
