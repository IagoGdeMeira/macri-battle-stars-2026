#ifndef ground_detection_system_h
#define ground_detection_system_h

#include "../../../domain/include/Entity/Entity.h"

#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/System/System.h"

#include <optional>
#include <vector>

struct CollisionEvent;

class GroundDetectionSystem : public System
{
public:
    explicit GroundDetectionSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    std::vector<CollisionEvent> collisions;

    bool isStaticPushbox(UpdateContext& ctx, Entity entity) const;
    void processGroundCollision(UpdateContext& ctx, Entity staticEntity, Entity dynamicCollider);
    bool isStandingOnGround(UpdateContext& ctx, Entity dynamicCollider, Entity staticEntity, Entity owner) const;
    std::optional<Entity> getGroundedOwner(UpdateContext& ctx, Entity colliderEntity) const;
};

#endif // ground_detection_system_h
