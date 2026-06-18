#ifndef state_transition_h
#define state_transition_h

#include "../ITriggerCondition/ITriggerCondition.h"

#include "../../domain/value_objects/StateId/StateId.h"
#include "../../domain/value_objects/TriggerId/TriggerId.h"

#include <memory>
#include <vector>

struct StateTransition
{
    StateId from, to;
    std::vector<TriggerId> triggers;
    std::vector<std::unique_ptr<ITriggerCondition>> conditions;
    int priority = 0;
};

#endif // state_transition_h
