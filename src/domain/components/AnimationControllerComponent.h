#ifndef animation_controller_component_h
#define animation_controller_component_h

#include "Animation/AnimationSet.h"
#include "StateId/StateId.h"

struct AnimationControllerComponent
{
    AnimationSet animations;
    StateId currentState = StateId::Unknown;
};

#endif // animation_controller_component_h
