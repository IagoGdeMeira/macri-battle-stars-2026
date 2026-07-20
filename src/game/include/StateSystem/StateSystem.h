#ifndef state_system_h
#define state_system_h

#include "StateMachine/StateMachine.h"
#include "StateMachineRegistry/StateMachineRegistry.h"
#include "TriggerEvent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <unordered_map>
#include <vector>

class StateSystem : public System
{
public:
    StateSystem(EventBus& eventBus, StateMachineRegistry& registry);
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
    StateMachineRegistry& registry;
    std::vector<TriggerEvent> events;

    bool hasTrigger(const StateTransition& transition, TriggerId trigger);
    bool conditionsAreValid(const StateTransition& transition, TriggerConditionContext& ctx);
    void processEntity(UpdateContext& ctx, Entity entity, const std::vector<TriggerId>& triggers);
};

#endif // state_system_h
