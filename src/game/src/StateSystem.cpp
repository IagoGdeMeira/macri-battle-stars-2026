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
    this->bus.subscribe<TriggerEvent>([this](const TriggerEvent& event)
    { this->events.push_back(event); });
}

void StateSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    auto stateView = View<StateComponent>(components);
    for (auto [entity, state] : stateView) state.timeInState += ctx.deltaTime;

    std::unordered_map<Entity, std::vector<TriggerId>, Entity::Hash> grouped;
    for (const auto& e : this->events) grouped[e.entity].push_back(e.trigger);

    auto machineView = View<StateComponent, StateMachineComponent>(components);
    for (auto [entity, state, machineComp] : machineView)
    {
        auto it = grouped.find(entity);
        const std::vector<TriggerId>* triggersPtr = (it != grouped.end()) ? &it->second : nullptr;

        TriggerConditionContext cctx{ ctx.world, entity, state };

        const StateTransition* best = nullptr;
        int bestPriority = std::numeric_limits<int>::min();

        for (const auto& transition : machineComp.machine.transitions)
        {
            if (transition.from != state.current) continue;
            if (!this->conditionsAreValid(transition, cctx)) continue;

            bool triggerMatched = false;
            if (!transition.triggers.empty())
            {
                if (triggersPtr) for (auto trigger : *triggersPtr)
                {
                    if (!this->hasTrigger(transition, trigger)) continue;
                    
                    triggerMatched = true;
                    break;
                }
                if (!triggerMatched) continue;
            }
            if (!best || transition.priority > bestPriority)
            {
                best = &transition;
                bestPriority = transition.priority;
            }
        }

        if (best)
        {
            StateId previous = state.current;
            state.current = best->to;
            state.timeInState = 0.f;
            this->bus.emit<StateChangedEvent>(StateChangedEvent{ entity, previous, state.current });
        }
    }

    this->events.clear();
}

bool StateSystem::hasTrigger(const StateTransition& transition, TriggerId trigger)
{
    auto& trgs = transition.triggers;
    return std::find(trgs.begin(), trgs.end(), trigger) != trgs.end();
}

bool StateSystem::conditionsAreValid(const StateTransition& transition, TriggerConditionContext& ctx)
{
    for (const auto& cond : transition.conditions) if (!cond->evaluate(ctx)) return false;
    return true;
}
