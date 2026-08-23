#include "HitboxControllerSystem/HitboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void HitboxControllerSystem::update(UpdateContext& ctx)
{
    auto& world = ctx.world;
    auto view = View<HitboxControllerComponent>(world.components());

    for (auto [entity, controller] : view) this->updateController({controller, world}, ctx.deltaTime);
}

void HitboxControllerSystem::updateController(const ControllerParams& params, float deltaTime)
{
    if (params.controller.frames.empty() || params.controller.finished) return;
    if (!params.controller.initialized) HitboxControllerSystem::initialize({params.controller, params.world});

    params.controller.elapsedTime += deltaTime;
    const auto& currentFrame = params.controller.frames[params.controller.currentFrame];
    float effectiveDuration = currentFrame.duration > 0.f ? currentFrame.duration : params.controller.frameDuration;

    if (params.controller.elapsedTime < effectiveDuration) return;

    HitboxControllerSystem::deactivateCurrentFrame(params);
    params.controller.elapsedTime = 0.f;

    if (!HitboxControllerSystem::advanceFrame(params)) params.controller.finished = true;
    else HitboxControllerSystem::activateCurrentFrame(params);
}

void HitboxControllerSystem::activateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].hitboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = true; }
}

void HitboxControllerSystem::deactivateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].hitboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void HitboxControllerSystem::activateCurrentFrame(const ControllerParams& params)
{ HitboxControllerSystem::activateFrame(params, params.controller.currentFrame); }

void HitboxControllerSystem::deactivateCurrentFrame(const ControllerParams& params)
{ HitboxControllerSystem::deactivateFrame(params, params.controller.currentFrame); }

void HitboxControllerSystem::initialize(const ControllerParams& params)
{
    if (params.controller.frames.empty() || params.controller.initialized) return;
    HitboxControllerSystem::activateFrame(params, params.controller.currentFrame);
    params.controller.initialized = true;
}

bool HitboxControllerSystem::advanceFrame(const ControllerParams& params)
{
    params.controller.currentFrame++;
    if (params.controller.currentFrame >= static_cast<int>(params.controller.frames.size()))
    {
        if (params.controller.loop)
        {
            params.controller.currentFrame = 0;
            return true;
        }
        else
        {
            params.controller.currentFrame = static_cast<int>(params.controller.frames.size()) - 1;
            return false;
        }
    }
    return true;
}
