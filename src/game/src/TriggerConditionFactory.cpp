#include "../include/TriggerConditionFactory/TriggerConditionFactory.h"

#include "../trigger_conditions/CompositeCondition.h"
#include "../trigger_conditions/HealthBelowCondition.h"
#include "../trigger_conditions/InputPressedCondition.h"
#include "../trigger_conditions/MinTimeCondition.h"
#include "../trigger_conditions/NotCondition.h"
#include "../trigger_conditions/VelocityAboveCondition.h"

#include "../../engine/include/InputMapper/InputMapper.h"

#include <stdexcept>

std::unique_ptr<ITriggerCondition> TriggerConditionFactory::create(const DataNode& node)
{
    using CType = CompositeCondition::CompositeType;

    std::string type = node.getString("type");

    if (type == "and" || type == "or")
    {
        std::vector<std::unique_ptr<ITriggerCondition>> children;
        CType ctype = (type == "and") ? CType::And : CType::Or;

        for (auto& child : node.getArray("conditions")) children.push_back(TriggerConditionFactory::create(*child));
        return std::make_unique<CompositeCondition>(ctype, std::move(children));
    }

    if (type == "not")
    {
        auto childNodes = node.getArray("condition");
        auto& childNode = childNodes.front();
        return std::make_unique<NotCondition>(TriggerConditionFactory::create(*childNode));
    }

    if (type == "min_time")         return std::make_unique<MinTimeCondition>(node.getFloat("value"));
    if (type == "velocity_above")   return std::make_unique<VelocityAboveCondition>(node.getFloat("value"));
    if (type == "health_below")     return std::make_unique<HealthBelowCondition>(node.getInt("value"));
    
    if (type == "input_pressed")
    {
        const auto action = InputMapper::stringToAction(node.getString("action"));
        if (action == InputAction::None)
        { throw std::runtime_error("Unknown input action in input_pressed condition"); }

        return std::make_unique<InputPressedCondition>(action);
    }

    throw std::runtime_error("Unknown condition type: " + type);
}
