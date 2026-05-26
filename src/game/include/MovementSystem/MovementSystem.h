#ifndef movement_system_h
#define movement_system_h

#include "../../../engine/include/System/System.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/ComponentManager/ComponentManager.h"
#include "../../domain/include/View/View.h"

class MovementSystem : public System
{
public:
    void update(UpdateContext& ctx) override
    {
        auto view = View<TransformComponent, VelocityComponent>(ctx.world.components());

        for (auto [entity, t, v] : view)
        {
            if (ctx.world.components().has<HitstopComponent>(entity))
            { if (ctx.world.components().get<HitstopComponent>(entity).frozen) continue; }

            t.x += v.vx * ctx.deltaTime;
            t.y += v.vy * ctx.deltaTime;
        }
    }
};

#endif // movement_system_h
