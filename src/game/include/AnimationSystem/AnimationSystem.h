#ifndef animation_system_h
#define animation_system_h

#include "engine/include/System/System.h"

class AnimationSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // animation_system_h
