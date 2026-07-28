#include "InputSystem/InputSystem.h"

#include "InputMapper/InputMapper.h"
#include "UpdateContext/UpdateContext.h"

#include "domain/components/AnalogInputComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

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

    for (const auto& e : this->digitalEvents)
    {
        this->sourceStates[e.playerId][e.source] = e.pressed;
        LOG_DEBUG("InputSystem: digital event player={} source={} pressed={}",
            e.playerId, InputMapper::sourceToString(e.source), e.pressed);
    }

    LOG_DEBUG("InputSystem: sourceStates after applying events:");
    for (const auto& [pid, states] : this->sourceStates)
    {
        LOG_DEBUG("  player {} has {} entries", pid, states.size());
        for (const auto& [src, pressed] : states)
            LOG_DEBUG("    {} = {}", InputMapper::sourceToString(src), pressed);
    }

    auto digitalView = View<InputComponent, PlayerComponent>(comp);
    for (auto [entity, input, player] : digitalView)
    {
        LOG_DEBUG("InputSystem: player {} entity {}", player.id, entity.id);

        auto bindingIt = this->context.bindings.find(player.id);
        if (bindingIt == this->context.bindings.end())
        {
            LOG_WARN("InputSystem: no binding found for player {}, skipping", player.id);
            continue;
        }

        auto& binding = bindingIt->second;
        auto& playerSources = this->sourceStates[player.id];
        auto& previousSources = this->previousSourceStates[player.id];

        LOG_DEBUG("InputSystem: player {} playerSources has {} entries", player.id, playerSources.size());
        for (const auto& [src, pressed] : playerSources) LOG_DEBUG("    {} = {}",
            InputMapper::sourceToString(src), pressed);

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

        LOG_DEBUG("InputSystem: player {} final actions after mapping:", player.id);
        for (const auto& [action, state] : input.actions) LOG_DEBUG("  {} pressed={} heldTime={:.3f}",
            InputMapper::actionToString(action), state.pressed, state.heldTime);

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
