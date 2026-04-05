#ifndef trigger_mapper_h
#define trigger_mapper_h

#include "../../domain/include/TriggerId/TriggerId.h"

#include <string>

class TriggerMapper
{
public:
    static TriggerId fromString(const std::string& triggerName);
};

#endif // trigger_mapper_h
