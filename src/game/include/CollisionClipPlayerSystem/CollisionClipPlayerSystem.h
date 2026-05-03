#ifndef collision_clip_player_system_h
#define collision_clip_player_system_h

#include "../EntityFactory/EntityFactory.h"
#include "../../events/StateChangedEvent.h"

#include "../../../domain/components/CollisionClipPlayerComponent.h"
#include "../../../domain/events/OrientationChangedEvent.h"

#include "../../../engine/include/System/System.h"
#include "../../../engine/include/EventBus/EventBus.h"

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

    std::unordered_map<Entity, std::vector<Entity>> activeColliders;

    std::vector<StateChangedEvent> stateChanges;
    std::vector<OrientationChangedEvent> orientationChanges;

    void refreshColliders(UpdateContext& ctx, Entity owner, CollisionClipPlayerComponent& player);
};

#endif // collision_clip_player_system_h
