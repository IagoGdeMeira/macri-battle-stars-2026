#ifndef animation_state_system_h
#define animation_state_system_h

#include "OrientationChangedEvent.h"
#include "StateChangedEvent.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Animation/Animation.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <unordered_set>
#include <vector>

class AnimationStateSystem : public System
{
public:
    AnimationStateSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    using OrientationSet = std::unordered_set<Entity, Entity::Hash>;

    EventBus& bus;

    std::vector<StateChangedEvent> stateChanges;
    std::vector<OrientationChangedEvent> orientationChanges;

    void processStateChanges(World& world);
    OrientationSet collectOrientationChanges(World& world);
    const Animation* selectAnimation(World& world, Entity entity) const;
    void applyAnimation(World& world, Entity entity, const Animation& chosen, bool resetProgress);
    void updateAnimations(World& world, const OrientationSet& orientChanges);
};

#endif // animation_state_system_h
