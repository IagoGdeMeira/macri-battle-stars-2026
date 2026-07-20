#ifndef crouch_system_h
#define crouch_system_h

#include "domain/components/InputComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/TriggerId/TriggerId.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <unordered_map>

class CrouchSystem : public System
{
public:
    explicit CrouchSystem(EventBus& bus) : bus(bus) {}
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;

    std::unordered_map<Entity, bool, Entity::Hash> wasCrouching;

    bool hasInputAction(InputComponent& input, InputAction action) const;
    bool isGrounded(UpdateContext& ctx, Entity entity) const;
};

#endif // crouch_system_h
