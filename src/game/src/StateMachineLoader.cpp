#include "../include/StateMachineLoader/StateMachineLoader.h"

#include "../include/StateIdMapper/StateIdMapper.h"
#include "../include/TriggerConditionFactory/TriggerConditionFactory.h"
#include "../include/TriggerMapper/TriggerMapper.h"

#include <stdexcept>
#include <unordered_set>

StateMachine StateMachineLoader::load(const std::string& path) const
{
    StateIdMapper localMapper;
    return this->load(path, localMapper);
}

StateMachine StateMachineLoader::load(const std::string& path, const StateIdMapper& mapper) const
{
    auto root = this->parser.parse(path);
    StateMachine machine;

    for (auto& node : root->getArray("transitions"))
    {
        StateTransition t;

        std::string fromStr = node->getString("from");
        std::string toStr = node->getString("to");

        t.from = mapper.fromString(fromStr);
        t.to = mapper.fromString(toStr);
        t.priority = node->has("priority") ? node->getInt("priority") : 0;

        if (t.from == StateId::Unknown) throw std::runtime_error("Invalid 'from' state: " + fromStr);
        if (t.to == StateId::Unknown) throw std::runtime_error("Invalid 'to' state: " + toStr);

        std::unordered_set<TriggerId> uniqueTriggers;
        if (node->has("triggers")) for (auto& tnode : node->getArray("triggers"))
        {
            std::string triggerStr = tnode->has("value") ? tnode->getString("value") : tnode->getString("");

            auto trigger = TriggerMapper::fromString(triggerStr);
            if (trigger == TriggerId::Unknown) throw std::runtime_error("Invalid trigger in transition from '"
                + fromStr + "' to '" + toStr + "': " + triggerStr);

            if (!uniqueTriggers.insert(trigger).second) throw std::runtime_error("Duplicate trigger in transition from '"
                + fromStr + "' to '" + toStr + "': " + triggerStr);

            t.triggers.push_back(trigger);
        }
        else if (node->has("trigger"))
        {
            std::string triggerStr = node->getString("trigger");
            auto trigger = TriggerMapper::fromString(triggerStr);
            if (trigger == TriggerId::Unknown) throw std::runtime_error("Invalid trigger in transition from '"
                + fromStr + "' to '" + toStr + "': " + triggerStr);

            t.triggers.push_back(trigger);
        }

        if (node->has("conditions")) for (auto& cnode : node->getArray("conditions"))
        { t.conditions.push_back(TriggerConditionFactory::create(*cnode)); }

        if (t.triggers.empty() && t.conditions.empty()) throw std::runtime_error("Transition from '"
            + fromStr + "' to '" + toStr + "' must have at least one trigger or condition");

        machine.transitions.push_back(std::move(t));
    }

    if (machine.transitions.empty()) throw std::runtime_error("StateMachine must contain at least one transition");

    std::unordered_set<StateId, StateId::Hash> usedStates;
    for (const auto& t : machine.transitions)
    {
        usedStates.insert(t.from);
        usedStates.insert(t.to);
    }

    for (const auto& state : usedStates) if (state == StateId::Unknown)
    { throw std::runtime_error("StateMachine contains Unknown state"); }

    return machine;
}
