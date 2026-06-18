#ifndef state_component_h
#define state_component_h

#include "../value_objects/StateId/StateId.h"

struct StateComponent
{
	StateId current = StateId::Idle;
	float timeInState = 0.f;
};

#endif // state_component_h
