#include "../include/AnimationStateSystem/AnimationStateSystem.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/utils/Logger/Logger.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

AnimationStateSystem::AnimationStateSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& event)
    { this->stateChanges.push_back(event); });

    this->bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& event)
    { this->orientationChanges.push_back(event); });
}

void AnimationStateSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& change : this->stateChanges)
    {
        if (!comp.has<AnimationControllerComponent>(change.entity)) continue;
        comp.get<AnimationControllerComponent>(change.entity).currentState = change.current;
        LOG_DEBUG("AnimationStateSystem: StateChangedEvent for entity {} -> state {}",
            change.entity.id, change.current.value());
    }
    this->stateChanges.clear();

    for (const auto& change : this->orientationChanges)
    {
        if (!comp.has<AnimationComponent>(change.entity)) continue;
        if (!comp.has<AnimationControllerComponent>(change.entity)) continue; 
        comp.get<AnimationComponent>(change.entity).currentState = StateId::Unknown;
        LOG_DEBUG("AnimationStateSystem: OrientationChangedEvent for entity {}, resetting animation state",
            change.entity.id);
    }
    this->orientationChanges.clear();

    auto view = View<AnimationComponent, AnimationControllerComponent>(comp);
    for (auto [entity, anim, controller] : view)
    {
        auto& animState = anim.currentState;
        auto& controllerState = controller.currentState;
        LOG_DEBUG("AnimationStateSystem: entity {} controller state = {} (value {}), anim state = {} (value {})",
            entity.id, controllerState.value(), controllerState.value(), animState.value(), animState.value());

        if (anim.currentState == controller.currentState) continue;

        auto itRight = controller.animations.right.find(controller.currentState);
        const Animation* chosen = (itRight != controller.animations.right.end()) ? &itRight->second : nullptr;

        if (chosen && comp.has<OrientationComponent>(entity))
        {
            const auto& orientation = comp.get<OrientationComponent>(entity);
            if (orientation.direction == Orientation::Left)
            {
                auto& leftAnim = controller.animations.left;
                auto itLeft = leftAnim.find(controller.currentState);
                if (itLeft != leftAnim.end()) chosen = &itLeft->second;
            }
        }
        
        if (!chosen)
        {
            LOG_DEBUG("AnimationStateSystem: no animation found for state {}", controller.currentState.value());
            continue;
        }

        LOG_DEBUG("AnimationStateSystem: entity {} switching to animation with {} frames",
            entity.id, chosen->frames.size());
        
        anim.animation      = *chosen;
        anim.currentFrame   = 0;
        anim.elapsedTime    = 0.f;
        anim.currentState   = controller.currentState;
    }
}
