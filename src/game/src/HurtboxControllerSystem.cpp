#include "HurtboxControllerSystem/HurtboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void HurtboxControllerSystem::update(UpdateContext& ctx)
{
    auto& world = ctx.world;
    auto view = View<HurtboxControllerComponent>(world.components());

    for (auto [entity, controller] : view) this->updateController({controller, world}, ctx.deltaTime);
}

void HurtboxControllerSystem::updateController(const ControllerParams& params, float deltaTime)
{
    if (params.controller.frames.empty() || params.controller.finished) return;
    if (!params.controller.initialized) HurtboxControllerSystem::initialize({params.controller, params.world});

    params.controller.elapsedTime += deltaTime;
    const auto& currentFrame = params.controller.frames[params.controller.currentFrame];
    float effectiveDuration = currentFrame.duration > 0.f ? currentFrame.duration : params.controller.frameDuration;

    if (params.controller.elapsedTime < effectiveDuration) return;

    HurtboxControllerSystem::deactivateCurrentFrame(params);
    params.controller.elapsedTime = 0.f;

    if (!HurtboxControllerSystem::advanceFrame(params))
    {
        HurtboxControllerSystem::activateCurrentFrame(params);
        params.controller.finished = true;
    }
    else HurtboxControllerSystem::activateCurrentFrame(params);
}

void HurtboxControllerSystem::activateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].hurtboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = true; }
}

void HurtboxControllerSystem::deactivateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].hurtboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void HurtboxControllerSystem::activateCurrentFrame(const ControllerParams& params)
{ HurtboxControllerSystem::activateFrame(params, params.controller.currentFrame); }

void HurtboxControllerSystem::deactivateCurrentFrame(const ControllerParams& params)
{ HurtboxControllerSystem::deactivateFrame(params, params.controller.currentFrame); }

void HurtboxControllerSystem::initialize(const ControllerParams& params)
{
    if (params.controller.frames.empty() || params.controller.initialized) return;
    HurtboxControllerSystem::activateFrame(params, params.controller.currentFrame);
    params.controller.initialized = true;
}

bool HurtboxControllerSystem::advanceFrame(const ControllerParams& params)
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
