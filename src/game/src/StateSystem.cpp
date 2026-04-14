#include "../include/StateSystem/StateSystem.h"

#include "../events/StateChangedEvent.h"

#include "../../domain/components/StateComponent.h"
#include "../../domain/components/StateMachineComponent.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <limits>
#include <unordered_map>

StateSystem::StateSystem(EventBus& eventBus) : bus(eventBus)
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
        auto& components = ctx.world.components();

        if (!components.has<StateMachineComponent>(entity)) continue;

        auto& state = components.get<StateComponent>(entity);
        auto& machine = components.get<StateMachineComponent>(entity).machine;

        ConditionContext cctx { ctx.world, entity, state };
        
        FindTransitionParams params { machine, state.current, e.trigger, cctx };
        const StateTransition* transition = this->findTransition(params);

        if (transition)
        {
            StateId previous = state.current;

            state.current = transition->to;
            state.timeInState = 0.0f;

            this->bus.emit<StateChangedEvent>(StateChangedEvent
            { entity, previous, state.current });
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
    for (const auto& cond : transition.conditions)
    { if (!cond->evaluate(ctx)) return false; }

    return true;
}

const StateTransition* StateSystem::findTransition(FindTransitionParams& params)
{
    const StateTransition* best = nullptr;
    int bestPriority = std::numeric_limits<int>::min();

    for (const auto& transition : params.stateMachine.transitions)
    {
        if (transition.from != params.currentState) continue;
        if (!this->hasTrigger(transition, params.trigger)) continue;
        if (!this->conditionsAreValid(transition, params.ctx)) continue;

        if (!best || transition.priority > bestPriority)
        {
            best = &transition;
            bestPriority = transition.priority;
        }
    }

    return best;
}
