#ifndef movement_system_h
#define movement_system_h

#include "engine/include/System/System.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "domain/components/HitstopComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"
#include "domain/include/View/View.h"

class MovementSystem : public System
{
public:
    void update(UpdateContext& ctx) override
    {
        auto& comp = ctx.world.components();
        auto view = View<TransformComponent, VelocityComponent>(comp);

        for (auto [entity, transform, velocity] : view)
        {
            if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

            auto& pos = transform.position;
            pos.x += velocity.velocity.x * ctx.deltaTime;
            pos.y += velocity.velocity.y * ctx.deltaTime;
        }
    }
};

#endif // movement_system_h
