#ifndef direction_trigger_system_h
#define direction_trigger_system_h

#include "domain/components/InputComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <unordered_map>

class DirectionTriggerSystem : public System
{
public:
    explicit DirectionTriggerSystem(EventBus& bus) : bus(bus) {}
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;

    std::unordered_map<Entity, bool, Entity::Hash> wasMovingLeft;
    std::unordered_map<Entity, bool, Entity::Hash> wasMovingRight;

    bool hasInputAction(InputComponent& input, InputAction action) const;
};

#endif // direction_trigger_system_h
