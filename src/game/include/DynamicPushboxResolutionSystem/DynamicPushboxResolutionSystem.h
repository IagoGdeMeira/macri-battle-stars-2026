#ifndef dynamic_pushbox_resolution_system_h
#define dynamic_pushbox_resolution_system_h

#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

#include <vector>

struct CollisionEvent;

class DynamicPushboxResolutionSystem : public System
{
public:
    explicit DynamicPushboxResolutionSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    std::vector<CollisionEvent> collisions;
    
    void resolveDynamicCollision(UpdateContext& ctx, Entity a, Entity b);
};

#endif // dynamic_pushbox_resolution_system_h
