#ifndef animation_state_system_h
#define animation_state_system_h

#include "StateChangedEvent.h"

#include "domain/events/OrientationChangedEvent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <vector>

class AnimationStateSystem : public System
{
public:
    AnimationStateSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;

    std::vector<StateChangedEvent> stateChanges;
    std::vector<OrientationChangedEvent> orientationChanges;
};

#endif // animation_state_system_h
