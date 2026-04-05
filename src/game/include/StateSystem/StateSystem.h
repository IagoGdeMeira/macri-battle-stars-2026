#ifndef state_system_h
#define state_system_h

#include "../StateMachine/StateMachine.h"
#include "../../events/ComboExecutedEvent.h"

#include "../../../domain/components/StateComponent.h"

#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/System/System.h"

#include <vector>

class StateSystem : public System
{
public:
    StateSystem(EventBus& eventBus, const StateMachine& stateMachine);

    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    const StateMachine& machine;

    std::vector<ComboExecutedEvent> events;

    bool hasTrigger(const StateTransition& transition, TriggerId trigger);

    bool conditionsAreValid(const StateTransition& transition, ConditionContext& ctx);
    
    const StateTransition* findTransition(
        const StateMachine& stateMachine,
        StateId currentState,
        TriggerId trigger,
        ConditionContext& ctx
    );
};

#endif // state_system_h
