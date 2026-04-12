#include "../include/AnimationStateSystem/AnimationStateSystem.h"

#include "../events/StateChangedEvent.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

AnimationStateSystem::AnimationStateSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& event)
    { this->changes.push_back(PendingChange{ event.entity, event.current }); });
}

void AnimationStateSystem::update(UpdateContext &ctx)
{
    auto& components = ctx.world.components();

    for (const auto &change : this->changes)
    {
        if (!components.has<AnimationControllerComponent>(change.entity)) continue;

        auto& controller = components.get<AnimationControllerComponent>(change.entity);
        controller.currentState = change.state;
    }

    this->changes.clear();

    auto view = View<AnimationComponent, AnimationControllerComponent>(components);
    for (auto [entity, anim, controller] : view)
    {
        if (anim.currentState == controller.currentState) continue;

        auto it = controller.animations.find(controller.currentState);
        if (it == controller.animations.end()) continue;

        anim.animation = it->second;
        anim.currentFrame = 0;
        anim.elapsedTime = 0.0f;
        anim.currentState = controller.currentState;
    }
}
