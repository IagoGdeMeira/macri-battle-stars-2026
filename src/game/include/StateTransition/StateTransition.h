#ifndef state_transition_h
#define state_transition_h

#include "../ITriggerCondition/ITriggerCondition.h"

#include "../../domain/include/StateId/StateId.h"
#include "../../domain/include/TriggerId/TriggerId.h"

#include <memory>
#include <vector>

struct StateTransition
{
    StateId from;
    StateId to;

    std::vector<TriggerId> triggers;
    
    std::vector<std::unique_ptr<ITriggerCondition>> conditions;

    int priority = 0;
};

#endif // state_transition_h
