#include "../include/InputBufferSystem/InputBufferSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

InputBufferSystem::InputBufferSystem(EventBus& bus, InputContext& context)
    : context(context)
{
    bus.subscribe<KeyEvent>([this](const KeyEvent& e)
    { if (e.pressed) events.push_back(e); });
}

void InputBufferSystem::update(UpdateContext& ctx)
{
    auto view = View<InputBufferComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, buffer, player] : view)
    {
        for (auto& entry : buffer.buffer) entry.time += ctx.deltaTime;

        while (!buffer.buffer.empty() && buffer.buffer.front().time > maxBufferTime)
        { buffer.buffer.pop_front(); }
    }

    for (const auto& e : events)
    {
        auto& binding = context.bindings[e.player];

        auto it = binding.keyMap.find(e.key);
        if (it == binding.keyMap.end()) continue;

        auto action = it->second;

        for (auto [entity, buffer, player] : view)
        {
            if (player.id != e.player) continue;
            buffer.buffer.push_back({ action, 0.f });
        }
    }

    events.clear();
}
