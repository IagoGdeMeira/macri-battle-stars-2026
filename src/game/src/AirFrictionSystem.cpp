#include "../include/AirFrictionSystem/AirFrictionSystem.h"

#include "../../domain/components/AirFrictionComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

void AirFrictionSystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GroundedComponent, AirFrictionComponent>(ctx.world.components());
    
    for (auto [entity, v, g, air] : view)
    {
        if (ctx.world.components().has<HitstopComponent>(entity))
        { if (ctx.world.components().get<HitstopComponent>(entity).frozen) continue; }

        if (g.onGround) continue;

        float effectiveX = this->airFriction * air.multiplierX;
        float effectiveY = this->airFriction * air.multiplierY;

        if (effectiveX < 0.0f) effectiveX = 0.0f;
        if (effectiveY < 0.0f) effectiveY = 0.0f;

        float decayX = 1.0f - (effectiveX * 0.01f * ctx.deltaTime);
        float decayY = 1.0f - (effectiveY * 0.01f * ctx.deltaTime);

        if (decayX < 0.0f) decayX = 0.0f;
        if (decayY < 0.0f) decayY = 0.0f;

        v.vx *= decayX;
        v.vy *= decayY;

        if (std::abs(v.vx) < 1.0f) v.vx = 0.0f;
        if (std::abs(v.vy) < 1.0f) v.vy = 0.0f;
    }
}
