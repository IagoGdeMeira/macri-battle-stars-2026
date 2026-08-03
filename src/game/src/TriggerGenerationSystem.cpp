#include "TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "TriggerEvent.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/include/InputMapper/InputMapper.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void TriggerGenerationSystem::update(UpdateContext& ctx) { this->processInputTriggers(ctx); }

void TriggerGenerationSystem::processInputTriggers(UpdateContext& ctx)
{
    auto view = View<InputComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, input, player] : view)
    {
        // LOG_DEBUG("TriggerGenerationSystem: processing entity {} player {}", entity.id, player.id);

        for (const auto& [action, state] : input.actions)
        {
            /*
            LOG_DEBUG("TriggerGenerationSystem: entity {} player {} action {} pressed={}",
                entity.id, player.id, InputMapper::actionToString(action), state.pressed);
            */
            if (!state.pressed) continue;

            auto it = this->context.bindings.find(action);
            if (it == this->context.bindings.end()) continue;

            for (auto trigger : it->second)
            {
                /*
                LOG_DEBUG("TriggerGenerationSystem: player {} action {} -> trigger {}",
                    player.id, InputMapper::actionToString(action), static_cast<int>(trigger));
                */
                this->bus.emit<TriggerEvent>(TriggerEvent{ entity, trigger });
            }
        }
    }
}