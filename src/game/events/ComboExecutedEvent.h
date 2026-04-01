#ifndef combo_executed_event_h
#define combo_executed_event_h

#include "../../domain/include/Entity/Entity.h"

#include <string>

struct ComboExecutedEvent
{
    Entity entity;
    std::string name;
};

#endif // combo_executed_event_h
