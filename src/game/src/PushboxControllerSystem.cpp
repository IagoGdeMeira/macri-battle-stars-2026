#include "PushboxControllerSystem/PushboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void PushboxControllerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    auto view = View<PushboxControllerComponent>(comp);
    for (auto [entity, controller] : view)
    {
        if (controller.frames.empty()) continue;

        controller.elapsedTime += ctx.deltaTime;
        auto& currentFrame = controller.frames[controller.currentFrame];

        if (controller.elapsedTime < currentFrame.duration) continue;
        
        for (Entity e : currentFrame.pushboxes)
        {
            if (!comp.has<ActiveComponent>(e)) continue;
            comp.get<ActiveComponent>(e).active = false;
        }

        controller.elapsedTime = 0.f;
        controller.currentFrame++;

        if (controller.currentFrame >= static_cast<int>(controller.frames.size()))
        {
            if (controller.loop) controller.currentFrame = 0;
            else controller.currentFrame = static_cast<int>(controller.frames.size()) - 1;
        }

        auto& newFrame = controller.frames[controller.currentFrame];
        for (Entity e : newFrame.pushboxes)
        {
            if (!comp.has<ActiveComponent>(e)) continue;
            comp.get<ActiveComponent>(e).active = true;
        }
    }
}
