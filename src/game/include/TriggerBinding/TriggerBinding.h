#ifndef trigger_binding_h
#define trigger_binding_h

#include "../../domain/include/InputAction/InputAction.h"
#include "../../domain/include/TriggerId/TriggerId.h"

#include <vector>

struct TriggerBinding
{
    InputAction action;
    std::vector<TriggerId> triggers;
};

#endif // trigger_binding_h
