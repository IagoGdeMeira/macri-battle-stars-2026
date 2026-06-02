#include "../include/ComboSystem/ComboSystem.h"

#include "../events/ComboExecutedEvent.h"
#include "../events/TriggerEvent.h"
#include "../include/TriggerMapper/TriggerMapper.h"

#include "../../domain/include/View/View.h"

void ComboSystem::update(UpdateContext& ctx)
{
    auto view = View<InputBufferComponent>(ctx.world.components());

    for (auto [entity, buffer] : view)
    {
        const Combo* best = nullptr;

        for (const auto& combo : this->combos)
        {
            if (!this->matches(buffer, combo)) continue;
            if (!best || combo.priority > best->priority) best = &combo;
        }

        if (best)
        {
            this->eventBus.emit<ComboExecutedEvent>(ComboExecutedEvent{entity, best->trigger});
            this->eventBus.emit<TriggerEvent>(TriggerEvent{entity, best->trigger});
            if (best->consumeInput) buffer.buffer.clear();
        }
    }
}

bool ComboSystem::matches(const InputBufferComponent& buffer, const Combo& combo)
{
    if (buffer.buffer.size() < combo.steps.size()) return false;

    size_t bi = buffer.buffer.size();
    size_t ci = combo.steps.size();

    float time = 0.f;

    while (ci > 0)
    {
        bi--;
        ci--;

        const auto& input = buffer.buffer[bi];
        const auto& step = combo.steps[ci];

        if (input.action != step.action) return false;

        time += input.time;

        if (time > step.maxDelay) return false;
    }
    return true;
}
