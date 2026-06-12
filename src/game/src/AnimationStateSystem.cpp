#include "../include/AnimationStateSystem/AnimationStateSystem.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

AnimationStateSystem::AnimationStateSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& event)
    { this->stateChanges.push_back(event); });

    this->bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& event)
    { this->orientationChanges.push_back(event); });
}

void AnimationStateSystem::update(UpdateContext &ctx)
{
    auto& components = ctx.world.components();

    for (const auto& change : this->stateChanges)
    {
        if (!components.has<AnimationControllerComponent>(change.entity)) continue;
        components.get<AnimationControllerComponent>(change.entity).currentState = change.current;
    }
    this->stateChanges.clear();

    for (const auto& oc : this->orientationChanges)
    {
        if (!components.has<AnimationComponent>(oc.entity)) continue;
        if (!components.has<AnimationControllerComponent>(oc.entity)) continue; 
        components.get<AnimationComponent>(oc.entity).currentState = StateId::Unknown;
    }
    this->orientationChanges.clear();

    auto view = View<AnimationComponent, AnimationControllerComponent>(components);
    for (auto [entity, anim, controller] : view)
    {
        if (anim.currentState == controller.currentState) continue;

        auto itRight = controller.animations.right.find(controller.currentState);
        const Animation* chosen = (itRight != controller.animations.right.end())
            ? &itRight->second : nullptr;

        if (chosen && components.has<OrientationComponent>(entity))
        {
            const auto& orientation = components.get<OrientationComponent>(entity);
            if (orientation.direction == Orientation::Left)
            {
                auto itLeft = controller.animations.left.find(controller.currentState);
                if (itLeft != controller.animations.left.end()) chosen = &itLeft->second;
            }
        }
        if (!chosen) continue;

        anim.animation    = *chosen;
        anim.currentFrame = 0;
        anim.elapsedTime  = 0.f;
        anim.currentState = controller.currentState;
    }
}
