#ifndef hold_trigger_system_h
#define hold_trigger_system_h

#include "domain/components/InputComponent.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <functional>
#include <unordered_map>

class HoldTriggerSystem : public System
{
public:
    using ConditionFunc = std::function<bool(UpdateContext&, Entity)>;

    struct Config
    {
        InputAction action;
        TriggerId onPress, onRelease = TriggerId::Unknown;
        ConditionFunc condition = nullptr;
    };

    HoldTriggerSystem(EventBus& bus, Config cfg) : bus(bus), cfg(cfg) {}

    void update(UpdateContext& ctx) override;

private:
    Config cfg;
    EventBus& bus;

    std::unordered_map<Entity, bool, Entity::Hash> wasPressed;

    bool hasInputAction(InputComponent& input) const;
};

#endif // hold_trigger_system_h
