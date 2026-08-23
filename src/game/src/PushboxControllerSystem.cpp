#include "PushboxControllerSystem/PushboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void PushboxControllerSystem::update(UpdateContext& ctx)
{
    auto& world = ctx.world;
    auto view = View<PushboxControllerComponent>(world.components());

    for (auto [entity, controller] : view) this->updateController({controller, world}, ctx.deltaTime);
}

void PushboxControllerSystem::updateController(const ControllerParams& params, float deltaTime)
{
    if (params.controller.frames.empty() || params.controller.finished) return;
    if (!params.controller.initialized) PushboxControllerSystem::initialize({params.controller, params.world});

    params.controller.elapsedTime += deltaTime;
    const auto& currentFrame = params.controller.frames[params.controller.currentFrame];
    float effectiveDuration = currentFrame.duration > 0.f ? currentFrame.duration : params.controller.frameDuration;

    if (params.controller.elapsedTime < effectiveDuration) return;

    PushboxControllerSystem::deactivateCurrentFrame(params);
    params.controller.elapsedTime = 0.f;

    if (!PushboxControllerSystem::advanceFrame(params))
    {
        PushboxControllerSystem::activateCurrentFrame(params);
        params.controller.finished = true;
    }
    else PushboxControllerSystem::activateCurrentFrame(params);
}

void PushboxControllerSystem::activateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].pushboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = true; }
}

void PushboxControllerSystem::deactivateFrame(const ControllerParams& params, int frameIndex)
{
    auto& comp = params.world.components();
    for (Entity e : params.controller.frames[frameIndex].pushboxes)
    { if (comp.has<ActiveComponent>(e)) comp.get<ActiveComponent>(e).active = false; }
}

void PushboxControllerSystem::activateCurrentFrame(const ControllerParams& params)
{ PushboxControllerSystem::activateFrame(params, params.controller.currentFrame); }

void PushboxControllerSystem::deactivateCurrentFrame(const ControllerParams& params)
{ PushboxControllerSystem::deactivateFrame(params, params.controller.currentFrame); }

void PushboxControllerSystem::initialize(const ControllerParams& params)
{
    if (params.controller.frames.empty() || params.controller.initialized) return;
    PushboxControllerSystem::activateFrame(params, params.controller.currentFrame);
    params.controller.initialized = true;
}

bool PushboxControllerSystem::advanceFrame(const ControllerParams& params)
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
