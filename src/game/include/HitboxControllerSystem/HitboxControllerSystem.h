#ifndef hitbox_controller_system_h
#define hitbox_controller_system_h

#include "engine/include/System/System.h"

class HitboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // hitbox_controller_system_h
