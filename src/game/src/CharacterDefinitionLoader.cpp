#include "CharacterDefinitionLoader/CharacterDefinitionLoader.h"

#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"

#include <stdexcept>

CharacterDefinition CharacterDefinitionLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);

    CharacterDefinition def;

    def.id = root->getString("id");

    auto sizeNode = root->has("spriteSize") ? root->getObject("spriteSize") : nullptr;
    def.spriteSize = (sizeNode) ? DataUtils::parseSize(*sizeNode) : Dimension2D{0, 0};

    def.texturePath = root->getString("texture", "");
    def.animationsPath = root->getString("animations", "");
    def.stateMachinePath = root->getString("stateMachine", "");
    def.combosPath = root->getString("combos", "");
    def.collisionsPath = root->getString("collisions", "");
    
    if (root->has("jump"))
    {
        auto jumpNode = root->getObject("jump");
        def.jump.force = jumpNode->getFloat("force", 1500.f);
        def.jump.maxTime = jumpNode->getFloat("maxTime", 0.2f);
        def.jump.gravityScaleAsc = jumpNode->getFloat("gravityScaleAsc", 0.6f);
        def.jump.gravityScaleDesc = jumpNode->getFloat("gravityScaleDesc", 1.8f);
        def.jump.fastFallMultiplier = jumpNode->getFloat("fastFallMultiplier", 2.5f);
    }

    if (root->has("health"))
    {
        auto healthNode = root->getObject("health");
        def.health.initial = healthNode->getInt("initial", 100);
        def.health.max = healthNode->getInt("max", 100);
    }

    if (root->has("customStates")) for (auto& node : root->getArray("customStates"))
    {
        const std::string customState = node->getString("");
        if (StateId::isBaseName(customState))
        { throw std::runtime_error("CharacterDefinition custom state collides with base state: " + customState); }

        def.customStates.push_back(customState);
    }

    if (def.id.empty()) throw std::runtime_error("CharacterDefinition missing id");
    return def;
}
