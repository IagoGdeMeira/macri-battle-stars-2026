#ifndef static_pushbox_resolution_system_h
#define static_pushbox_resolution_system_h

#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

#include <vector>

struct CollisionEvent;

class StaticPushboxResolutionSystem : public System
{
public:
    explicit StaticPushboxResolutionSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    std::vector<CollisionEvent> collisions;
    
    void resolveStaticCollision(UpdateContext& ctx, Entity dyn, Entity sta);
};

#endif // static_pushbox_resolution_system_h
