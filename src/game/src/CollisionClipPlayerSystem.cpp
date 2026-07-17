#include "../include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"

#include "../events/StateChangedEvent.h"

#include "../../domain/events/OrientationChangedEvent.h"
#include "../../domain/components/CollisionClipPlayerComponent.h"
#include "../../domain/components/CollisionClipDefinitionsComponent.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

CollisionClipPlayerSystem::CollisionClipPlayerSystem(EventBus& bus, EntityFactory& factory) :
    bus(bus), factory(factory)
{
    bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
    { this->stateChanges.push_back(e); });

    bus.subscribe<OrientationChangedEvent>([this](const OrientationChangedEvent& e)
    { this->orientationChanges.push_back(e); });
}

void CollisionClipPlayerSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    for (const auto& sc : this->stateChanges)
    {
        if (!components.has<CollisionClipPlayerComponent>(sc.entity)) continue;
        if (!components.has<CollisionClipDefinitionsComponent>(sc.entity)) continue;

        auto& player = components.get<CollisionClipPlayerComponent>(sc.entity);
        auto& defs = components.get<CollisionClipDefinitionsComponent>(sc.entity);

        auto it = defs.clips.find(sc.current);
        if (it != defs.clips.end() && it->second)
        {
            player.currentClip = it->second;
            player.currentFrame = 0;
            player.elapsedTime = 0.f;
            player.playing = true;
        }
        else player.playing = false;
        
        this->refreshColliders(ctx, sc.entity);
    }
    this->stateChanges.clear();

    auto view = View<CollisionClipPlayerComponent>(components);
    for (auto [entity, player] : view)
    {
        if (!player.playing || !player.currentClip) continue;
        if (player.currentClip->frames.empty()) continue;

        player.elapsedTime += ctx.deltaTime;
        const auto& frame = player.currentClip->frames[player.currentFrame];

        if (player.elapsedTime < frame.duration) continue;
        
        player.elapsedTime -= frame.duration;
        player.currentFrame++;

        if (player.currentFrame >= static_cast<int>(player.currentClip->frames.size()))
        {
            if (player.currentClip->loop) player.currentFrame = 0;
            else
            {
                player.playing = false;
                player.currentFrame = static_cast<int>(player.currentClip->frames.size()) - 1;
            }
        }
        this->refreshColliders(ctx, entity);    
    }

    for (const auto& oc : this->orientationChanges)
    {
        if (!components.has<CollisionClipPlayerComponent>(oc.entity)) continue; 
        if (!components.has<CollisionClipDefinitionsComponent>(oc.entity)) continue;

        auto& player = components.get<CollisionClipPlayerComponent>(oc.entity);
        if (player.playing) this->refreshColliders(ctx, oc.entity);
    }
    this->orientationChanges.clear();
}

void CollisionClipPlayerSystem::refreshColliders(UpdateContext& ctx, Entity owner)
{
    auto& components = ctx.world.components();
    auto& player = components.get<CollisionClipPlayerComponent>(owner);

    auto it = this->activeColliders.find(owner);
    if (it != this->activeColliders.end())
    {
        for (Entity child : it->second) ctx.world.entities().destroy(child);
        it->second.clear();
    }
    if (!player.playing || !player.currentClip) return;

    bool facingLeft = components.has<OrientationComponent>(owner)
        ? (components.get<OrientationComponent>(owner).direction == Orientation::Left)
        : false;

    const auto& frame = player.currentClip->frames[player.currentFrame];
    std::vector<Entity> newChildren;

    for (const auto& hb : frame.hitboxes)
    { newChildren.push_back(this->factory.createHitbox(owner, hb, facingLeft)); }

    for (const auto& hb : frame.hurtboxes)
    { newChildren.push_back(this->factory.createHurtbox(owner, hb, facingLeft)); }

    for (const auto& pb : frame.pushboxes)
    { newChildren.push_back(this->factory.createPushbox(owner, pb, facingLeft)); }

    this->activeColliders[owner] = std::move(newChildren);
}
