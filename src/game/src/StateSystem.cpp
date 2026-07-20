#include "StateSystem/StateSystem.h"

#include "StateChangedEvent.h"

#include "domain/components/StateComponent.h"
#include "domain/components/StateMachineComponent.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <limits>
#include <unordered_map>

StateSystem::StateSystem(EventBus& eventBus, StateMachineRegistry& registry) : bus(eventBus), registry(registry)
{
    this->bus.subscribe<TriggerEvent>([this](const TriggerEvent& event)
    { this->events.push_back(event); });
}

void StateSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    auto view = View<StateComponent>(comp);
    for (auto [entity, state] : view) state.timeInState += ctx.deltaTime;
    
    std::unordered_map<Entity, std::vector<TriggerId>, Entity::Hash> grouped;
    for (const auto& e : this->events) grouped[e.entity].push_back(e.trigger);
    for (const auto& entry : grouped) this->processEntity(ctx, entry.first, entry.second);

    this->events.clear();
}

void StateSystem::processEntity(UpdateContext& ctx, Entity entity, const std::vector<TriggerId>& triggers)
{
    auto& comp = ctx.world.components();

    if (!comp.has<StateComponent>(entity) || !comp.has<StateMachineComponent>(entity)) return;

    auto& state = comp.get<StateComponent>(entity);
    uint32_t machineId = comp.get<StateMachineComponent>(entity).machineId;

    const StateMachine* machine = this->registry.getMachine(machineId);
    if (!machine) return;

    TriggerConditionContext cctx{ ctx.world, entity, state };

    const StateTransition* best = nullptr;
    int bestPriority = std::numeric_limits<int>::min();

    for (const auto& transition : machine->transitions)
    {
        if (transition.from != state.current) continue;
        if (!this->conditionsAreValid(transition, cctx)) continue;

        bool matched = false;
        for (auto trigger : triggers) if (this->hasTrigger(transition, trigger))
        { matched = true; break; }

        if (!matched) continue;

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
