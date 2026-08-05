#include "InputSystem/InputSystem.h"

#include "InputMapper/InputMapper.h"
#include "UpdateContext/UpdateContext.h"

#include "domain/components/AnalogInputComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"

#include <map>

InputSystem::InputSystem(EventBus& bus, InputContext& inputContext) : context(inputContext)
{
    bus.subscribe<DigitalInputEvent>([this](const DigitalInputEvent& event)
    { this->digitalEvents.push_back(event); });

    bus.subscribe<AnalogInputEvent>([this](const AnalogInputEvent& event)
    { this->analogEvents.push_back(event); });
}

void InputSystem::update(UpdateContext& ctx)
{
    if (!this->digitalEvents.empty()) this->hasProcessedThisFrame = false;
    if (!this->digitalEvents.empty() && !this->hasProcessedThisFrame) this->processDigitalEvents(ctx);

    this->processAnalogEvents(ctx);

    this->digitalEvents.clear();
    this->analogEvents.clear();
}

void InputSystem::processDigitalEvents(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& e : this->digitalEvents) this->sourceStates[e.playerId][e.source] = e.pressed;

    auto digitalView = View<InputComponent, PlayerComponent>(comp);
    for (auto [entity, input, player] : digitalView)
    {
        auto bindingIt = this->context.bindings.find(player.id);
        if (bindingIt == this->context.bindings.end()) continue;

        auto& binding = bindingIt->second;
        auto& playerSources = this->sourceStates[player.id];
        auto& previousSources = this->previousSourceStates[player.id];

        std::map<InputAction, bool> actionPressed;

        for (const auto& [source, action] : binding.keyMap)
        {
            bool isPressed = playerSources.count(source) && playerSources.at(source);
            if (isPressed) actionPressed[action] = true;
        }

        for (const auto& [source, action] : binding.keyMap)
        {
            bool isPressed = playerSources.count(source) && playerSources.at(source);
            bool wasPressed = previousSources.count(source) && previousSources.at(source);

            auto& state = input.actions[action];
            state.pressed = actionPressed[action];
            if (isPressed && !wasPressed) state.heldTime = 0.f;
        }

        for (auto& [action, state] : input.actions) if (state.pressed) state.heldTime += ctx.deltaTime;

        previousSources = playerSources;
    }

    this->hasProcessedThisFrame = true;
}

void InputSystem::processAnalogEvents(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto analogView = View<AnalogInputComponent, PlayerComponent>(comp);

    for (auto [entity, analog, player] : analogView) analog.move = {0.f, 0.f};

    for (auto& e : this->analogEvents)
    {
        if (e.source.type() == InputSource::Type::Mouse) continue;

        for (auto [entity, analog, player] : analogView)
        {
            if (player.id != e.playerId) continue;
            analog.move.x = e.value;
        }
    }
}
