#ifndef animation_set_h
#define animation_set_h

#include "Animation.h"
#include "StateId/StateId.h"

#include <unordered_map>

struct AnimationSet
{
    using AnimationMap = std::unordered_map<StateId, Animation, StateId::Hash>;
    
    AnimationMap right, left;
    bool symmetric = true;
};

#endif // animation_set_h
