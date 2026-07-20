#include "TriggerBindingLoader/TriggerBindingLoader.h"

#include "TriggerMapper/TriggerMapper.h"

#include "engine/include/InputMapper/InputMapper.h"

TriggerContext TriggerBindingLoader::load(const std::string& path)
{
    auto root = this->parser.parse(path);

    TriggerContext context;

    for (auto& node : root->getArray("bindings"))
    {
        auto action = InputMapper::stringToAction(node->getString("action"));

        std::vector<TriggerId> triggers;

        for (auto& tnode : node->getArray("triggers"))
        {
            auto trigger = TriggerMapper::fromString(tnode->getString("trigger"));
            triggers.push_back(trigger);
        }

        context.bindings[action] = std::move(triggers);
    }

    return context;
}
