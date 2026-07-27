#include "CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

CollisionControllerSystem::CollisionControllerSystem(EventBus& bus) : bus(bus)
{
    bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
    { this->pendingEvents.push_back(e); });
}

void CollisionControllerSystem::update(UpdateContext& ctx)
{
    World& world = ctx.world;
    auto& comp = world.components();

    for (const auto& e : this->pendingEvents)
    {
        if (comp.has<HitboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<HitboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) this->applyHitboxController({world, it->second}, e.entity);
            else if (comp.has<HitboxControllerComponent>(e.entity))
            {
                this->deactivateCurrentFrame({world, comp.get<HitboxControllerComponent>(e.entity)});
                comp.remove<HitboxControllerComponent>(e.entity);
            }
        }

        if (comp.has<HurtboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<HurtboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) this->applyHurtboxController({world, it->second}, e.entity);
            else if (comp.has<HurtboxControllerComponent>(e.entity))
            {
                this->deactivateCurrentFrame({world, comp.get<HurtboxControllerComponent>(e.entity)});
                comp.remove<HurtboxControllerComponent>(e.entity);
            }
        }

        if (comp.has<PushboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<PushboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) this->applyPushboxController({world, it->second}, e.entity);
            else if (comp.has<PushboxControllerComponent>(e.entity))
            {
                this->deactivateCurrentFrame({world, comp.get<PushboxControllerComponent>(e.entity)});
                comp.remove<PushboxControllerComponent>(e.entity);
            }
        }
    }
    this->pendingEvents.clear();

    auto initView = View<StateComponent>(comp);
    for (auto [entity, state] : initView)
    {
        StateId current = state.current;

        if (comp.has<HitboxControllerMapComponent>(entity) && !comp.has<HitboxControllerComponent>(entity))
        {
            auto& mapComp = comp.get<HitboxControllerMapComponent>(entity);
            auto it = mapComp.map.find(current);
            if (it != mapComp.map.end()) this->applyHitboxController({world, it->second}, entity);
        }

        if (comp.has<HurtboxControllerMapComponent>(entity) && !comp.has<HurtboxControllerComponent>(entity))
        {
            auto& mapComp = comp.get<HurtboxControllerMapComponent>(entity);
            auto it = mapComp.map.find(current);
            if (it != mapComp.map.end()) this->applyHurtboxController({world, it->second}, entity);
        }

        if (comp.has<PushboxControllerMapComponent>(entity) && !comp.has<PushboxControllerComponent>(entity))
        {
            auto& mapComp = comp.get<PushboxControllerMapComponent>(entity);
            auto it = mapComp.map.find(current);
            if (it != mapComp.map.end()) this->applyPushboxController({world, it->second}, entity);
        }
    }
}

void CollisionControllerSystem::activateFirstFrame(const HitboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[0].hitboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void CollisionControllerSystem::deactivateCurrentFrame(const HitboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || !controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[controller.currentFrame].hitboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void CollisionControllerSystem::applyHitboxController(const HitboxParams& params, Entity entity)
{
    auto& comp = params.world.components();
    if (comp.has<HitboxControllerComponent>(entity))
    {
        auto& old = comp.get<HitboxControllerComponent>(entity);
        this->deactivateCurrentFrame({params.world, old});
        old.frames = params.controller.frames;
        old.loop = params.controller.loop;
        old.currentFrame = 0;
        old.elapsedTime = 0.f;
        old.initialized = false;
        this->activateFirstFrame({params.world, old});
    }
    else
    {
        auto controller = params.controller;
        this->activateFirstFrame({params.world, controller});
        comp.add<HitboxControllerComponent>(entity, std::move(controller));
    }
}

void CollisionControllerSystem::activateFirstFrame(const HurtboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[0].hurtboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void CollisionControllerSystem::deactivateCurrentFrame(const HurtboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || !controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[controller.currentFrame].hurtboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void CollisionControllerSystem::applyHurtboxController(const HurtboxParams& params, Entity entity)
{
    auto& comp = params.world.components();
    if (comp.has<HurtboxControllerComponent>(entity))
    {
        auto& old = comp.get<HurtboxControllerComponent>(entity);
        this->deactivateCurrentFrame({params.world, old});
        old.frames = params.controller.frames;
        old.loop = params.controller.loop;
        old.currentFrame = 0;
        old.elapsedTime = 0.f;
        old.initialized = false;
        this->activateFirstFrame({params.world, old});
    }
    else
    {
        auto controller = params.controller;
        this->activateFirstFrame({params.world, controller});
        comp.add<HurtboxControllerComponent>(entity, std::move(controller));
    }
}

void CollisionControllerSystem::activateFirstFrame(const PushboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[0].pushboxes) if (comp.has<ActiveComponent>(e))
    { comp.get<ActiveComponent>(e).active = true; }
    controller.initialized = true;
}

void CollisionControllerSystem::deactivateCurrentFrame(const PushboxParams& params)
{
    auto& controller = params.controller;
    if (controller.frames.empty() || !controller.initialized) return;

    auto& comp = params.world.components();
    for (Entity e : controller.frames[controller.currentFrame].pushboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void CollisionControllerSystem::applyPushboxController(const PushboxParams& params, Entity entity)
{
    auto& comp = params.world.components();
    if (comp.has<PushboxControllerComponent>(entity))
    {
        auto& old = comp.get<PushboxControllerComponent>(entity);
        this->deactivateCurrentFrame({params.world, old});
        old.frames = params.controller.frames;
        old.loop = params.controller.loop;
        old.currentFrame = 0;
        old.elapsedTime = 0.f;
        old.initialized = false;
        this->activateFirstFrame({params.world, old});
    }
    else
    {
        auto controller = params.controller;
        this->activateFirstFrame({params.world, controller});
        comp.add<PushboxControllerComponent>(entity, std::move(controller));
    }
}
