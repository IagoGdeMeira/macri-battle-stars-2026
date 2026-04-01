#include "../include/ComboLoader/ComboLoader.h"

#include "../../engine/include/InputMapper/InputMapper.h"

std::vector<Combo> ComboLoader::load(const std::string& path)
{
    auto root = this->parser.parse(path);

    std::vector<Combo> combos;

    for (auto& comboNode : root->getArray("combos"))
    {
        Combo combo;

        combo.name = comboNode->getString("name");
        
        if (comboNode->has("priority")) combo.priority = comboNode->getInt("priority");
        if (comboNode->has("consume")) combo.consumeInput = comboNode->getBool("consume");

        for (auto& stepNode : comboNode->getArray("steps"))
        {
            ComboStep step;

            step.action = InputMapper::stringToAction(stepNode->getString("action"));
            step.maxDelay = stepNode->getFloat("maxDelay");

            combo.steps.push_back(step);
        }
        combos.push_back(combo);
    }
    return combos;
}
