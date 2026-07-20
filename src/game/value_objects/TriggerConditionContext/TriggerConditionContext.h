#ifndef trigger_condition_context_h
#define trigger_condition_context_h

#include "domain/components/StateComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

struct TriggerConditionContext { World& world; Entity& entity; StateComponent& stateComponent; };

#endif // trigger_condition_context_h
