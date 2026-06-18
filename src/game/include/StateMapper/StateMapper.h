#ifndef state_mapper_h
#define state_mapper_h

#include "../../domain/value_objects/StateId/StateId.h"

#include <string>

class StateMapper
{
public:
    static StateId fromString(const std::string& stateName);
};

#endif // state_mapper_h
