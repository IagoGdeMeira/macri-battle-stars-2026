#ifndef pushbox_controller_system_h
#define pushbox_controller_system_h

#include "engine/include/System/System.h"

class PushboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // pushbox_controller_system_h
