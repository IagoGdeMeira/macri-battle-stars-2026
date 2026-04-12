#include "../include/StateSystem/StateSystem.h"

#include "../events/StateChangedEvent.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <unordered_map>

StateSystem::StateSystem(EventBus& eventBus, const StateMachine& stateMachine) :
    bus(eventBus), machine(stateMachine)
{
    bus.subscribe<TriggerEvent>([this](const TriggerEvent& event)
    { this->events.push_back(event); });
}

void StateSystem::update(UpdateContext& ctx)
{
    auto view = View<StateComponent>(ctx.world.components());

    for (auto [entity, state] : view) state.timeInState += ctx.deltaTime;

    for (const auto& e : this->events)
    {
        auto entity = e.entity;
        auto& state = ctx.world.components().get<StateComponent>(entity);

        ConditionContext cctx { ctx.world, entity, state };

        const StateTransition* transition = this->findTransition(
            this->machine, state.current, e.trigger, cctx);

        if (transition)
        {
            StateId previous = state.current;

            state.current = transition->to;
            state.timeInState = 0.0f;

            this->bus.emit<StateChangedEvent>(
                StateChangedEvent{ entity, previous, state.current });
        }
    }

    this->events.clear();
}

bool StateSystem::hasTrigger(const StateTransition& transition, TriggerId trigger)
{
    return std::find(
        transition.triggers.begin(),
        transition.triggers.end(),
        trigger
    ) != transition.triggers.end();
}

bool StateSystem::conditionsAreValid(const StateTransition& transition, ConditionContext& ctx)
{
    for (const auto& cond : transition.conditions) if (!cond->evaluate(ctx)) return false;

    return true;
}

const StateTransition* StateSystem::findTransition(
    const StateMachine& stateMachine,
    StateId currentState,
    TriggerId trigger,
    ConditionContext& ctx
) {
    for (const auto& transition : stateMachine.transitions)
    {
        if (transition.from != currentState) continue;
        if (!this->hasTrigger(transition, trigger)) continue;
        if (!this->conditionsAreValid(transition, ctx)) continue;

        return& transition;
    }

    return nullptr;
}
