#ifndef state_component_h
#define state_component_h

#include "../include/StateId/StateId.h"

struct StateComponent
{
	StateId current = StateId::Idle;
	float timeInState = 0.0f;
};

#endif // state_component_h
