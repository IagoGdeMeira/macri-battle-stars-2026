#include "../include/ConditionFactory/ConditionFactory.h"

#include "../conditions/MinTimeCondition.h"

#include <stdexcept>

std::unique_ptr<ICondition> ConditionFactory::create(const DataNode& node)
{
    std::string type = node.getString("type");

    if (type == "min_time") return std::make_unique<MinTimeCondition>(node.getFloat("value"));

    throw std::runtime_error("Unknown condition type: " + type);
}
