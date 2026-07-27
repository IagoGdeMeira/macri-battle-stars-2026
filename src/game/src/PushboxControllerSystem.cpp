#include "PushboxControllerSystem/PushboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void PushboxControllerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<PushboxControllerComponent>(comp);
    for (auto [entity, controller] : view)
    {
        if (controller.frames.empty()) continue;

        if (!controller.initialized)
        {
            for (Entity e : controller.frames[controller.currentFrame].pushboxes) if (comp.has<ActiveComponent>(e))
            {
                comp.get<ActiveComponent>(e).active = true;
                LOG_DEBUG("PushboxControllerSystem: entity {} frame {} activated child {}",
                    entity.id, controller.currentFrame, e.id);
            }
            controller.initialized = true;
        }

        controller.elapsedTime += ctx.deltaTime;
        auto& currentFrame = controller.frames[controller.currentFrame];

        if (controller.elapsedTime < currentFrame.duration) continue;
        
        for (Entity e : currentFrame.pushboxes) if (comp.has<ActiveComponent>(e))
        {
            comp.get<ActiveComponent>(e).active = false;
            LOG_DEBUG("PushboxControllerSystem: entity {} frame {} deactivated child {}",
                entity.id, controller.currentFrame, e.id);
        }

        controller.elapsedTime = 0.f;
        controller.currentFrame++;
        if (controller.currentFrame >= static_cast<int>(controller.frames.size()))
        {
            if (controller.loop) controller.currentFrame = 0;
            else controller.currentFrame = static_cast<int>(controller.frames.size()) - 1;
        }

        auto& newFrame = controller.frames[controller.currentFrame];
        for (Entity e : newFrame.pushboxes) if (comp.has<ActiveComponent>(e))
        {
            comp.get<ActiveComponent>(e).active = true;
            LOG_DEBUG("PushboxControllerSystem: entity {} advanced to frame {} activated child {}",
                entity.id, controller.currentFrame, e.id);
        }
    }
}
