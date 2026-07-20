#ifndef attack_system_h
#define attack_system_h

#include "domain/components/InputComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

class AttackSystem : public System
{
public:
    AttackSystem(EventBus& bus, InputAction action, TriggerId trigger) :
        bus(bus), action(action), trigger(trigger) {}

    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    InputAction action;
    TriggerId trigger;

    bool canAttack(UpdateContext& ctx, Entity entity) const;
    bool hasInputAction(InputComponent& input, InputAction inputAction) const;
};

#endif // attack_system_h
