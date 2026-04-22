#include "../include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"

#include "../../domain/include/StateId/StateId.h"

#include <stdexcept>

CharacterDefinition CharacterDefinitionLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);

    CharacterDefinition def;

    def.id = root->getString("id");

    def.texturePath = root->getString("texture");
    def.spriteWidth = root->getInt("spriteWidth");
    def.spriteHeight = root->getInt("spriteHeight");

    def.animationsPath = root->getString("animations");
    def.stateMachinePath = root->getString("stateMachine");

    if (root->has("combos")) def.combosPath = root->getString("combos");

    if (root->has("customStates")) for (auto& node : root->getArray("customStates"))
    {
        const auto customState = node->getString("");
        if (StateId::isBaseName(customState))
        { throw std::runtime_error("CharacterDefinition custom state collides with base state: " + customState); }

        def.customStates.push_back(customState);
    }

    if (def.id.empty()) throw std::runtime_error("CharacterDefinition missing id");

    return def;
}
