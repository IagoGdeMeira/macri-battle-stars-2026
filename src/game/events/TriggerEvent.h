#ifndef trigger_event_h
#define trigger_event_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/include/TriggerId/TriggerId.h"

struct TriggerEvent
{
    Entity entity;
    TriggerId trigger;
};

#endif // trigger_event_h
