#ifndef state_changed_event_h
#define state_changed_event_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/include/StateId/StateId.h"

struct StateChangedEvent
{
    Entity entity;
    StateId previous;
    StateId current;
};

#endif // state_changed_event_h
