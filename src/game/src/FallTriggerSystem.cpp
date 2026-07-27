#include "FallTriggerSystem/FallTriggerSystem.h"

#include "TriggerEvent.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void FallTriggerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<GroundedComponent, PlayerComponent>(comp);

    for (auto [entity, grounded, player] : view)
    {
        bool isGrounded = grounded.onGround;
        bool entityGrounded = this->wasGrounded[entity];

        if (entityGrounded && !isGrounded)
        {
            LOG_DEBUG("FallTriggerSystem: player {} Fall", player.id);
            this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Fall});
        }

        this->wasGrounded[entity] = isGrounded;
    }
}
