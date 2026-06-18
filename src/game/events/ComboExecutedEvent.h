#ifndef combo_executed_event_h
#define combo_executed_event_h

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/value_objects/TriggerId/TriggerId.h"

struct ComboExecutedEvent { Entity entity; TriggerId trigger; };

#endif // combo_executed_event_h
