#include "../include/ComboLoader/ComboLoader.h"

#include "../include/TriggerMapper/TriggerMapper.h"

#include "../../engine/include/InputMapper/InputMapper.h"

#include <stdexcept>

std::vector<Combo> ComboLoader::load(const std::string& path)
{
    auto root = this->parser.parse(path);

    std::vector<Combo> combos;

    for (auto& cnode : root->getArray("combos"))
    {
        Combo combo;

        combo.name = cnode->getString("name");
        combo.trigger = TriggerMapper::fromString(cnode->getString("trigger"));

        if (cnode->has("priority")) combo.priority = cnode->getInt("priority");
        if (cnode->has("consume")) combo.consumeInput = cnode->getBool("consume");

        for (auto& snode : cnode->getArray("steps"))
        {
            ComboStep step;

            step.action = InputMapper::stringToAction(snode->getString("action"));
            step.maxDelay = snode->getFloat("maxDelay");

            combo.steps.push_back(step);
        }
        if (combo.steps.empty())
        { throw std::runtime_error("Combo '" + combo.name + "' must have at least one step"); }

        combos.push_back(combo);
    }
    
    return combos;
}
