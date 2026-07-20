#include "FrictionSystem/FrictionSystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <cmath>

void FrictionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<VelocityComponent, GroundedComponent>(comp);
    
    for (auto [entity, velocity, grounded] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;
        if (!grounded.onGround) continue;

        float effectiveFriction = this->friction * (1.f - grounded.frictionReduction);
        if (effectiveFriction < 0.f) effectiveFriction = 0.f;

        float decay = 1.f - (effectiveFriction * 0.01f * ctx.deltaTime);
        if (decay < 0.f) decay = 0.f;

        velocity.velocity.x *= decay;

        if (std::abs(velocity.velocity.x) < 1.f) velocity.velocity.x = 0.f;
    }
}
