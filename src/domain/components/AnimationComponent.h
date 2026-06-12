#ifndef animation_component_h
#define animation_component_h

#include "../include/Animation/Animation.h"
#include "../include/StateId/StateId.h"

struct AnimationComponent
{
    Animation animation;

    float elapsedTime = 0.f;
    int currentFrame = 0;

    StateId currentState = StateId::Unknown;
};

#endif // animation_component_h
