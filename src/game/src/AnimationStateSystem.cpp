#include "../include/AnimationStateSystem/AnimationStateSystem.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

AnimationStateSystem::AnimationStateSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& event)
    { this->stateChanges.push_back(event); });

    this->bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& event)
    { this->orientationChanges.push_back(event); });
}

void AnimationStateSystem::update(UpdateContext& ctx)
{
    this->processStateChanges(ctx.world);
    auto orientChanges = this->collectOrientationChanges(ctx.world);
    this->updateAnimations(ctx.world, orientChanges);
}

void AnimationStateSystem::processStateChanges(World& world)
{
    auto& comp = world.components();
    for (const auto& change : this->stateChanges)
    {
        if (!comp.has<AnimationControllerComponent>(change.entity)) continue;
        comp.get<AnimationControllerComponent>(change.entity).currentState = change.current;
    }
    this->stateChanges.clear();
}

AnimationStateSystem::OrientationSet AnimationStateSystem::collectOrientationChanges(World& world)
{
    auto& comp = world.components();
    OrientationSet entities;
    for (const auto& change : this->orientationChanges)
    {
        if (!comp.has<AnimationControllerComponent>(change.entity)) continue;
        entities.insert(change.entity);
    }
    this->orientationChanges.clear();
    return entities;
}

const Animation* AnimationStateSystem::selectAnimation(World& world, Entity controllerEntity) const
{
    auto& comp = world.components();
    if (!comp.has<AnimationControllerComponent>(controllerEntity)) return nullptr;
    const auto& ctrl = comp.get<AnimationControllerComponent>(controllerEntity);

    const Animation* chosen = nullptr;
    auto itRight = ctrl.animations.right.find(ctrl.currentState);
    if (itRight != ctrl.animations.right.end()) chosen = &itRight->second;

    if (comp.has<OrientationComponent>(controllerEntity))
    {
        const auto& orient = comp.get<OrientationComponent>(controllerEntity);
        if (orient.direction == Orientation::Left)
        {
            auto itLeft = ctrl.animations.left.find(ctrl.currentState);
            if (itLeft != ctrl.animations.left.end()) chosen = &itLeft->second;
        }
    }
    return chosen;
}

void AnimationStateSystem::applyAnimation(World& world, Entity visualEntity, const Animation& chosen, bool resetProgress)
{
    auto& comp = world.components();
    auto& anim = comp.get<AnimationComponent>(visualEntity);
    auto& current = anim.currentAnimation;
    
    bool animationChanged = false;
    animationChanged |= (current.frames != chosen.frames);
    animationChanged |= (current.frameDuration != chosen.frameDuration);
    animationChanged |= (current.loop != chosen.loop);

    if (animationChanged)
    {
        anim.currentAnimation = chosen;
        resetProgress = true;
    }

    if (resetProgress)
    {
        anim.currentFrame = 0;
        anim.elapsedTime = 0.f;
    }

    if (comp.has<ParentComponent>(visualEntity))
    {
        Entity parent = comp.get<ParentComponent>(visualEntity).parent;
        if (comp.has<AnimationControllerComponent>(parent))
        {
            const auto& ctrl = comp.get<AnimationControllerComponent>(parent);
            anim.currentState = ctrl.currentState;
        }
    }
}

void AnimationStateSystem::updateAnimations(World& world, const OrientationSet& orientChanges)
{
    auto& comp = world.components();

    auto controllerView = View<AnimationControllerComponent, ChildrenComponent>(comp);
    for (auto [controllerEntity, ctrl, children] : controllerView)
    {
        bool orientationChanged = orientChanges.count(controllerEntity) > 0;

        for (Entity child : children.children)
        {
            if (!comp.has<AnimationComponent>(child)) continue;
            auto& anim = comp.get<AnimationComponent>(child);

            bool needsUpdate = false;
            needsUpdate |= (anim.currentAnimation.frames.empty());
            needsUpdate |= (anim.currentState != ctrl.currentState);
            needsUpdate |= orientationChanged;
            if (!needsUpdate) continue;

            const Animation *chosen = this->selectAnimation(world, controllerEntity);
            if (!chosen) continue;

            bool stateChanged = (anim.currentState != ctrl.currentState);
            bool animationDiffers = (*chosen != anim.currentAnimation);
            bool resetProgress = stateChanged || (orientationChanged && animationDiffers);

            this->applyAnimation(world, child, *chosen, resetProgress);
        }
    }
}
