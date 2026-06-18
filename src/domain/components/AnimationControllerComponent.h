#ifndef animation_controller_component_h
#define animation_controller_component_h

#include "../value_objects/Animation/AnimationSet.h"
#include "../value_objects/StateId/StateId.h"

struct AnimationControllerComponent
{
    AnimationSet animations;
    StateId currentState = StateId::Unknown;
};

#endif // animation_controller_component_h
