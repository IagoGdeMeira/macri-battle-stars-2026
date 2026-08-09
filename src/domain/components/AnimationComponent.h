#ifndef animation_component_h
#define animation_component_h

#include "Animation/Animation.h"
#include "StateId/StateId.h"

struct AnimationComponent
{
    float elapsedTime = 0.f;
    int currentFrame = 0;
    StateId currentState = StateId::Unknown;
    Animation currentAnimation;
};

#endif // animation_component_h
