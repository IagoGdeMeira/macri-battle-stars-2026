#include "../include/InputBindingLoader/InputBindingLoader.h"

#include "../include/InputMapper/InputMapper.h"

InputContext InputBindingLoader::load(const std::string& path)
{
    auto root = this->parser.parse(path);
    InputContext context;

    for (auto& playerNode : root->getArray("players"))
    {
        uint32_t playerId = static_cast<uint32_t>(playerNode->getInt("id"));
        InputBinding binding;

        for (auto& b : playerNode->getArray("bindings"))
        {
            auto action = InputMapper::stringToAction(b->getString("action"));
            auto source = InputMapper::stringToSource(b->getString("source"));
            binding.keyMap[source] = action;
        }

        context.bindings[playerId] = std::move(binding);
    }

    return context;
}
