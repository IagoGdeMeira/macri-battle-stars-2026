#include "InputBufferSystem/InputBufferSystem.h"

#include "UpdateContext/UpdateContext.h"

#include "domain/components/InputBufferComponent.h"
#include "domain/components/PlayerComponent.h"

InputBufferSystem::InputBufferSystem(EventBus& bus, InputContext& context) : context(context)
{
    bus.subscribe<DigitalInputEvent>([this](const DigitalInputEvent& e)
    { if (e.pressed) this->events.push_back(e); });
}

void InputBufferSystem::update(UpdateContext& ctx)
{
    auto view = View<InputBufferComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, buffer, player] : view)
    {
        for (auto& entry : buffer.buffer) entry.time += ctx.deltaTime;

        while (!buffer.buffer.empty() && buffer.buffer.front().time > this->maxBufferTime)
        { buffer.buffer.pop_front(); }
    }

    for (const auto& e : this->events)
    {
        auto& binding = this->context.bindings[e.playerId];
        auto it = binding.keyMap.find(e.source);
        if (it == binding.keyMap.end()) continue;

        InputAction action = it->second;

        for (auto [entity, buffer, player] : view)
        {
            if (player.id != e.playerId) continue;
            buffer.buffer.push_back({action, 0.f});
        }
    }
    this->events.clear();
}
