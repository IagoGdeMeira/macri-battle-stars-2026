#ifndef player_control_system_h
#define player_control_system_h

#include "../../domain/components/InputComponent.h"
#include "../../domain/include/Entity/Entity.h"
#include "../../domain/value_objects/InputAction/InputAction.h"
#include "../../domain/value_objects/StateId/StateId.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

#include <unordered_map>

class PlayerControlSystem : public System
{
public:
    PlayerControlSystem(EventBus& bus, float moveSpeed = 300.f, float jumpImpulse = -500.f) : 
        bus(bus), moveSpeed(moveSpeed), jumpImpulse(jumpImpulse) {}
    
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    float moveSpeed;
    float jumpImpulse;

    std::unordered_map<Entity, bool, Entity::Hash> wasCrouching;

    bool canMove(StateId state) const;
    bool hasInputAction(InputComponent& input, InputAction action) const; 

    /*
    void applyMovement(UpdateContext& ctx, Entity entity, bool canMove) const;
    void applyJump(UpdateContext& ctx, Entity entity) const;
    void emitTriggers(UpdateContext& ctx, Entity entity, bool canMove);
    */
};

#endif // player_control_system_h
