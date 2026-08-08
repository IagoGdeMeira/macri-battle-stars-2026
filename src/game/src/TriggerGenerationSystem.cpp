#include "TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "TriggerEvent.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/InputMapper/InputMapper.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void TriggerGenerationSystem::update(UpdateContext& ctx) { this->processInputTriggers(ctx); }

void TriggerGenerationSystem::processInputTriggers(UpdateContext& ctx)
{
    auto view = View<InputComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, input, p_] : view) for (const auto& [action, state] : input.actions)
    {
        (void)p_;
        if (!state.pressed) continue;

        auto it = this->context.bindings.find(action);
        if (it == this->context.bindings.end()) continue;

        for (auto trigger : it->second) this->bus.emit<TriggerEvent>(TriggerEvent{ entity, trigger });
    }
}
