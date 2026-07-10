#ifndef state_machine_loader_h
#define state_machine_loader_h

#include "../StateMachine/StateMachine.h"

#include "../../domain/value_objects/StateId/StateId.h"

#include "../../engine/include/DataParser/DataParser.h"

#include <functional>
#include <string>

class StateIdMapper;

class StateMachineLoader
{
public:
    StateMachineLoader(DataParser& parser) : parser(parser) {}

    StateMachine load(const std::string& path) const;
    StateMachine load(const std::string& path, const StateIdMapper& mapper) const;

private:
    DataParser& parser;
};

#endif // state_machine_loader_h
