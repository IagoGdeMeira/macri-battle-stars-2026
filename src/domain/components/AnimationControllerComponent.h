#ifndef animation_controller_component_h
#define animation_controller_component_h

#include "../include/Animation/Animation.h"
#include "../include/StateId/StateId.h"

#include <unordered_map>

struct AnimationControllerComponent
{
    std::unordered_map<StateId, Animation> animations;

    StateId currentState = StateId::Unknown;
};

#endif // animation_controller_component_h
