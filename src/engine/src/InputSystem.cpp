#include "../include/InputSystem/InputSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/include/View/View.h"

InputSystem::InputSystem(EventBus& bus, InputContext& inputContext) : context(inputContext)
{ bus.subscribe<KeyEvent>([this](const KeyEvent& event) { this->events.push_back(event); }); }

void InputSystem::update(UpdateContext& ctx)
{
    for (const auto& event : this->events)
    { this->keyStates[event.player][event.key] = event.pressed; }

    auto view = View<InputComponent, PlayerComponent>(ctx.world.components());

    for (auto [entity, input, player] : view)
    {
        auto& binding = this->context.bindings[player.id];
        auto& playerKeys = this->keyStates[player.id];
        auto& previousKeys = this->previousKeyStates[player.id];

        for (auto& [action, state] : input.actions)
        {
            state.pressed = false;
            state.heldTime += ctx.deltaTime;
        }

        for (const auto& [key, action] : binding.keyMap)
        {
            const bool isPressed = playerKeys.contains(key)
                && playerKeys.at(key);
            const bool wasPressed = previousKeys.contains(key)
                && previousKeys.at(key);

            auto& state = input.actions[action];
            state.pressed = isPressed;

            if (isPressed && !wasPressed) state.heldTime = 0.f;
        }

        previousKeys = playerKeys;
    }
    
    this->events.clear();
}
