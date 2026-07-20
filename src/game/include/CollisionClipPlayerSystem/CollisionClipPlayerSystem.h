#ifndef collision_clip_player_system_h
#define collision_clip_player_system_h

#include "EntityFactory/EntityFactory.h"
#include "StateChangedEvent.h"

#include "domain/components/CollisionClipPlayerComponent.h"
#include "domain/events/OrientationChangedEvent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"
#include "engine/include/EventBus/EventBus.h"

#include <unordered_map>
#include <vector>

class CollisionClipPlayerSystem : public System
{
public:
    CollisionClipPlayerSystem(EventBus& bus, EntityFactory& factory);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    EntityFactory& factory;

    std::unordered_map<Entity, std::vector<Entity>, Entity::Hash> activeColliders;

    std::vector<StateChangedEvent> stateChanges;
    std::vector<OrientationChangedEvent> orientationChanges;

    void refreshColliders(UpdateContext& ctx, Entity entity);
};

#endif // collision_clip_player_system_h
