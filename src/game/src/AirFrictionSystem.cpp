#include "AirFrictionSystem/AirFrictionSystem.h"

#include "domain/components/AirFrictionComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <cmath>

void AirFrictionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();    
    auto view = View<VelocityComponent, GroundedComponent, AirFrictionComponent>(comp);
    
    for (auto [entity, v, g, air] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;
        if (g.onGround) continue;

        auto& airF = this->airFriction;

        Position effective = {airF * air.multiplier.x, airF * air.multiplier.y};
        this->setNegativePositionToZero(effective);

        Position decay = {1.f - (effective.x * 0.01f * ctx.deltaTime), 1.f - (effective.y * 0.01f * ctx.deltaTime)};
        this->setNegativePositionToZero(decay);

        v.velocity.x *= decay.x;
        v.velocity.y *= decay.y;

        if (std::abs(v.velocity.x) < 1.f) v.velocity.x = 0.f;
        if (std::abs(v.velocity.y) < 1.f) v.velocity.y = 0.f;
    }
}

void AirFrictionSystem::setNegativePositionToZero(Position& pos)
{
    if (pos.x < 0.f) pos.x = 0.f;
    if (pos.y < 0.f) pos.y = 0.f;
}
