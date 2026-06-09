#include "../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../events/TriggerEvent.h"

#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void TriggerGenerationSystem::update(UpdateContext& ctx) { this->processInputTriggers(ctx); }

void TriggerGenerationSystem::processInputTriggers(UpdateContext& ctx)
{
    auto view = View<InputComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, input, player] : view)
    {
        for (const auto& [action, state] : input.actions)
        {
            if (!state.pressed) continue;

            auto it = this->context.bindings.find(action);
            if (it == this->context.bindings.end()) continue;

            for (auto trigger : it->second)
            { this->bus.emit<TriggerEvent>(TriggerEvent{ entity, trigger }); }
        }
    }
}
