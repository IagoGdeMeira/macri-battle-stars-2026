#ifndef state_machine_h
#define state_machine_h

#include "StateTransition/StateTransition.h"

#include <vector>

struct StateMachine { std::vector<StateTransition> transitions; };

#endif // state_machine_h
