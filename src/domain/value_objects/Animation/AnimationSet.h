#ifndef animation_set_h
#define animation_set_h

#include "Animation.h"

#include "../StateId/StateId.h"

#include <unordered_map>

struct AnimationSet
{
    std::unordered_map<StateId, Animation, StateId::Hash> right;
    std::unordered_map<StateId, Animation, StateId::Hash> left;
    bool symmetric = true;
};

#endif // animation_set_h
