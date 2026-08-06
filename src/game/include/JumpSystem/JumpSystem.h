#ifndef jump_system_h
#define jump_system_h

#include "domain/components/InputComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/Entity/Entity.h"

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
    void startJump(JumpComponent& jump, VelocityComponent& velocity, Entity entity);
    void applyJumpForce(JumpComponent& jump, VelocityComponent& velocity, float deltaTime, bool jumpHeld, Entity entity);
    void stopJump(JumpComponent& jump);
};

#endif // jump_system_h
