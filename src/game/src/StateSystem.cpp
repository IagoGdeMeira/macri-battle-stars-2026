#include "../include/StateSystem/StateSystem.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>

StateSystem::StateSystem(EventBus& eventBus, const StateMachine& stateMachine) :
    bus(eventBus), machine(stateMachine)
{
    bus.subscribe<ComboExecutedEvent>([this](const ComboExecutedEvent& event)
    { this->events.push_back(event); });
}

void StateSystem::update(UpdateContext& ctx)
{
    auto view = View<StateComponent>(ctx.world.components());

    for (auto [entity, state] : view) state.timeInState += ctx.deltaTime;

    for (const auto& e : this->events) for (auto [entity, state] : view)
    {
        if (entity != e.entity) continue;

        ConditionContext cctx { ctx.world, entity, state };

        const StateTransition* transition = findTransition(
            this->machine,
            state.current,
            e.trigger,
            cctx
        );

        if (transition != nullptr)
        {
            state.current = transition->to;
            state.timeInState = 0.0f;
        }

        break;
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
