#include "../include/InputSystem/InputSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/include/View/View.h"

InputSystem::InputSystem(EventBus& bus, InputContext& inputContext) :
    context(inputContext)
{
    bus.subscribe<KeyEvent>([this](const KeyEvent& event)
    { this->events.push_back(event); });
}

void InputSystem::update(UpdateContext& ctx)
{
    auto view = View<InputComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, input, player] : view)
    {
        auto& binding = this->context.bindings[player.id];

        for (auto& [action, state] : input.actions)
        {
            state.pressed = false;
            state.heldTime += ctx.deltaTime;
        }

        for (const auto& e : this->events)
        {
            if (e.player != player.id) continue;

            auto it = binding.keyMap.find(e.key);
            if (it == binding.keyMap.end()) continue;

            auto action = it->second;

            auto& state = input.actions[action];
            state.pressed = e.pressed;

            if (e.pressed) state.heldTime = 0.f;
        }
    }
    
    this->events.clear();
}
