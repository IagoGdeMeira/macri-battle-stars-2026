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
        for (const auto& e : events)
        {
            if (e.player != player.id) continue;
            
            auto& binding = this->context.bindings[e.player];
            auto it = binding.keyMap.find(e.key);

            if (it == binding.keyMap.end()) continue;

            auto& action = it->second;
            input.actions[action].pressed = e.pressed;

            if (!e.pressed) input.actions[action].heldTime = 0.0f;
        }
    }
    
    events.clear();
}
