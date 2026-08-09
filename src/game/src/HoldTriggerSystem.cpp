#include "HoldTriggerSystem/HoldTriggerSystem.h"

#include "TriggerEvent.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/InputMapper/InputMapper.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void HoldTriggerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, PlayerComponent>(comp);

    for (auto [entity, input, player] : view)
    {
        bool pressedNow = this->hasInputAction(input);
        bool pressedBefore = this->wasPressed[entity];

        bool cond = true;
        if (this->condition) cond = this->condition(ctx, entity);

        if (pressedNow && !pressedBefore && cond)
        {
            this->bus.emit<TriggerEvent>(TriggerEvent{entity, this->onPress});
            this->wasPressed[entity] = true;
        }
        else if (!pressedNow && pressedBefore && this->onRelease != TriggerId::Unknown)
        {
            this->bus.emit<TriggerEvent>(TriggerEvent{entity, this->onRelease});
            this->wasPressed[entity] = false;
        }
    }
}

bool HoldTriggerSystem::hasInputAction(InputComponent& input) const
{
    auto it = input.actions.find(this->action);
    return it != input.actions.end() && it->second.pressed;
}
