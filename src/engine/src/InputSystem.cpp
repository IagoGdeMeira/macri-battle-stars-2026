#include "../include/InputSystem/InputSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/include/View/View.h"

InputSystem::InputSystem(EventBus& bus, InputContext& inputContext) : context(inputContext)
{
    bus.subscribe<DigitalInputEvent>([this](const DigitalInputEvent& event)
    { this->digitalEvents.push_back(event); });

    bus.subscribe<AnalogInputEvent>([this](const AnalogInputEvent& event)
    { this->analogEvents.push_back(event); });
}

void InputSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& e : this->digitalEvents) this->sourceStates[e.playerId][e.source] = e.pressed;

    auto digitalView = View<InputComponent, PlayerComponent>(comp);
    for (auto [entity, input, player] : digitalView)
    {
        auto& binding = context.bindings[player.id];
        auto& playerSources = sourceStates[player.id];
        auto& previousSources = previousSourceStates[player.id];

        for (auto& [action, state] : input.actions)
        {
            state.pressed = false;
            state.heldTime += ctx.deltaTime;
        }

        for (const auto& [source, action] : binding.keyMap)
        {
            bool isPressed = playerSources.count(source) && playerSources.at(source);
            bool wasPressed = previousSources.count(source) && previousSources.at(source);

            auto& state = input.actions[action];
            state.pressed = isPressed;
            if (isPressed && !wasPressed) state.heldTime = 0.f;
        }

        previousSources = playerSources;
    }

    auto analogView = View<AnalogInputComponent, PlayerComponent>(comp);
    for (auto& e : this->analogEvents)
    {
        for (auto [entity, analog, player] : analogView)
        {
            if (player.id != e.playerId) continue;

            if (e.source.type() == InputSource::Type::Mouse || e.source.type() == InputSource::Type::Gamepad)
            { analog.moveX += e.value; }
        }
    }

    this->digitalEvents.clear();
    this->analogEvents.clear();
}
