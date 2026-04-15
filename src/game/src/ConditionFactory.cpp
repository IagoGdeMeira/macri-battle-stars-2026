#include "../include/ConditionFactory/ConditionFactory.h"

#include "../conditions/CompositeCondition.h"
#include "../conditions/HealthBelowCondition.h"
#include "../conditions/InputPressedCondition.h"
#include "../conditions/MinTimeCondition.h"
#include "../conditions/NotCondition.h"
#include "../conditions/VelocityAboveCondition.h"

#include "../../engine/include/InputMapper/InputMapper.h"

#include <stdexcept>

std::unique_ptr<ICondition> ConditionFactory::create(const DataNode& node)
{
    using CType = CompositeCondition::CompositeType;

    std::string type = node.getString("type");

    if (type == "and" || type == "or")
    {
        std::vector<std::unique_ptr<ICondition>> children;
        CType ctype = (type == "and") ? CType::And : CType::Or;

        for (auto& child : node.getArray("conditions"))
        {  children.push_back(ConditionFactory::create(*child)); }

        return std::make_unique<CompositeCondition>(ctype, std::move(children));
    }

    if (type == "not")
    {
        auto childNodes = node.getArray("condition");
        auto& childNode = childNodes.front();
        return std::make_unique<NotCondition>(ConditionFactory::create(*childNode));
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
