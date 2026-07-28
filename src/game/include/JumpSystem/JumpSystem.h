#ifndef jump_system_h
#define jump_system_h

#include "domain/components/InputComponent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

class JumpSystem : public System
{
public:
    JumpSystem(EventBus& bus, float jumpImpulse = -500.f) : bus(bus), jumpImpulse(jumpImpulse) {}
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    float jumpImpulse;

    bool hasInputAction(InputComponent& input, InputAction action) const;
};

#endif // jump_system_h
