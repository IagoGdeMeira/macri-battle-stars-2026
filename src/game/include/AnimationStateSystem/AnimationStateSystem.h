#ifndef animation_state_system_h
#define animation_state_system_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/include/StateId/StateId.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

#include <vector>

class AnimationStateSystem : public System
{
public:
    AnimationStateSystem(EventBus& bus);

    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;

    struct PendingChange
    {
        Entity entity;
        StateId state;
    };

    std::vector<PendingChange> changes;
};

#endif // animation_state_system_h
