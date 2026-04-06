#include "../include/StateMachineLoader/StateMachineLoader.h"

#include "../include/ConditionFactory/ConditionFactory.h"
#include "../include/StateMapper/StateMapper.h"
#include "../include/TriggerMapper/TriggerMapper.h"

#include <stdexcept>
#include <unordered_set>

StateMachine StateMachineLoader::load(const std::string& path) const
{
    auto root = parser.parse(path);

    StateMachine machine;

    for (auto& node : root->getArray("transitions"))
    {
        StateTransition t;

        const auto& fromStr = node->getString("from");
        const auto& toStr = node->getString("to");

        t.from = StateMapper::fromString(node->getString("from"));
        t.to = StateMapper::fromString(node->getString("to"));

        if (t.from == StateId::Unknown) throw std::runtime_error("Invalid 'from' state: " + fromStr);
        if (t.to == StateId::Unknown) throw std::runtime_error("Invalid 'to' state: " + toStr);

        std::unordered_set<TriggerId> uniqueTriggers;
        if (node->has("triggers")) for (auto& tnode : node->getArray("triggers"))
        {
            std::string triggerStr = tnode->has("value")
                ? tnode->getString("value")
                : tnode->getString("");

            auto trigger = TriggerMapper::fromString(triggerStr);

            if (trigger == TriggerId::Unknown) throw std::runtime_error(
                "Invalid trigger in transition from '" +
                fromStr + "' to '" +
                toStr + "': " + triggerStr);

            if (!uniqueTriggers.insert(trigger).second) throw std::runtime_error(
                "Duplicate trigger in transition from '" +
                fromStr + "' to '" +
                toStr + "': " + triggerStr);

            t.triggers.push_back(trigger);
        }
        else if (node->has("trigger"))
        {
            const auto triggerStr = node->getString("trigger");
            auto trigger = TriggerMapper::fromString(triggerStr);

            if (trigger == TriggerId::Unknown) throw std::runtime_error(
                "Invalid trigger in transition from '" +
                fromStr + "' to '" +
                toStr + "': " + triggerStr);

            t.triggers.push_back(trigger);
        }

        if (t.triggers.empty()) throw std::runtime_error(
            "Transition from '" +
            fromStr + "' to '" +
            toStr + "' must have at least one trigger");

        if (node->has("conditions")) for (auto& cnode : node->getArray("conditions"))
        { t.conditions.push_back(ConditionFactory::create(*cnode)); }

        machine.transitions.push_back(std::move(t));
    }

    if (machine.transitions.empty())
    { throw std::runtime_error("StateMachine must contain at least one transition"); }

    std::unordered_set<StateId> usedStates;

    for (const auto& t : machine.transitions)
    {
        usedStates.insert(t.from);
        usedStates.insert(t.to);
    }

    for (const auto& state : usedStates) if (state == StateId::Unknown)
    { throw std::runtime_error("StateMachine contains Unknown state"); }

    return machine;
}
