#include "../include/DirectionTriggerSystem/DirectionTriggerSystem.h"

#include "../events/TriggerEvent.h"

#include "../../domain/components/PlayerComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/InputAction/InputAction.h"
#include "../../domain/value_objects/TriggerId/TriggerId.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void DirectionTriggerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, PlayerComponent>(comp);

    for (auto [entity, input, player] : view)
    {
        bool moveLeft = this->hasInputAction(input, InputAction::MoveLeft);
        bool moveRight = this->hasInputAction(input, InputAction::MoveRight);

        bool wasLeft = this->wasMovingLeft[entity];
        bool wasRight = this->wasMovingRight[entity];

        if (moveLeft && !wasLeft) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::MoveLeft});
        else if (!moveLeft && wasLeft) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::MoveLeftReleased});

        if (moveRight && !wasRight) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::MoveRight});
        else if (!moveRight && wasRight) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::MoveRightReleased});

        this->wasMovingLeft[entity] = moveLeft;
        this->wasMovingRight[entity] = moveRight;
    }
}

bool DirectionTriggerSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}
