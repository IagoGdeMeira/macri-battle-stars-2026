#ifndef animation_controller_component_h
#define animation_controller_component_h

#include "../include/Animation/AnimationSet.h"
#include "../include/StateId/StateId.h"

#include <unordered_map>

struct AnimationControllerComponent
{
    AnimationSet animations;
    StateId currentState = StateId::Unknown;
};

#endif // animation_controller_component_h
