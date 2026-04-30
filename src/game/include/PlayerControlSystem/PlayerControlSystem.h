#ifndef player_control_system_h
#define player_control_system_h

#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/System/System.h"

class PlayerControlSystem : public System
{
public:
    PlayerControlSystem(EventBus& bus, float moveSpeed = 300.0f, float jumpImpulse = -500.0f);
    
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    float moveSpeed;
    float jumpImpulse;
};

#endif