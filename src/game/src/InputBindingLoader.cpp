#include "../include/InputBindingLoader/InputBindingLoader.h"

#include "../../engine/include/InputMapper/InputMapper.h"

InputContext InputBindingLoader::load(const std::string& path)
{
    auto root = parser.parse(path);

    InputContext context;

    for (auto& playerNode : root->getArray("players"))
    {
        PlayerId playerId = playerNode->getInt("id");

        InputBinding binding;

        for (auto& b : playerNode->getArray("bindings"))
        {
            auto action = InputMapper::stringToAction(b->getString("action"));
            auto key = InputMapper::stringToKey(b->getString("key"));

            binding.keyMap[key] = action;
        }

        context.bindings[playerId] = std::move(binding);
    }

    return context;
}
