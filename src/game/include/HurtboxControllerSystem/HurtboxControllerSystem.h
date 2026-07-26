#ifndef hurtbox_controller_system_h
#define hurtbox_controller_system_h

#include "engine/include/System/System.h"

class HurtboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // hurtbox_controller_system_h
