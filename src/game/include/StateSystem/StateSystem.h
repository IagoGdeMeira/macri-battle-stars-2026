#ifndef state_system_h
#define state_system_h

#include "../StateMachine/StateMachine.h"

#include "../events/TriggerEvent.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

#include <unordered_map>
#include <vector>

class StateSystem : public System
{
public:
    StateSystem(EventBus& eventBus);

    void update(UpdateContext& ctx) override;

private:
    struct FindTransitionParams 
    {
        const StateMachine& stateMachine;
        StateId currentState;
        const std::vector<TriggerId>& triggers;
        TriggerConditionContext& ctx;
    };

    EventBus& bus;
    std::vector<TriggerEvent> events;

    bool hasTrigger(const StateTransition& transition, TriggerId trigger);
    bool conditionsAreValid(const StateTransition& transition, TriggerConditionContext& ctx);
};

#endif // state_system_h
