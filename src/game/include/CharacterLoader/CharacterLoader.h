#ifndef character_loader_h
#define character_loader_h

#include "../AnimationLoader/AnimationLoader.h"
#include "../CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../StateMachineLoader/StateMachineLoader.h"
#include "../TextureLoader/TextureLoader.h"

#include "../../domain/include/World/World.h"

#include "../../engine/include/ResourceManager/ResourceManager.h"

class CharacterLoader
{
public:
    CharacterLoader(
        CharacterDefinitionLoader& defLoader,
        AnimationLoader& animLoader,
        StateMachineLoader& fsmLoader,
        ResourceManager& resourceManager,
        TextureLoader& textureLoader
    ) : 
        defLoader(defLoader),
        animLoader(animLoader),
        fsmLoader(fsmLoader),
        resourceManager(resourceManager),
        textureLoader(textureLoader) {}

    Entity create(World& world, const std::string& path) const;

private:
    CharacterDefinitionLoader& defLoader;
    AnimationLoader& animLoader;
    StateMachineLoader& fsmLoader;

    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
};

#endif // character_loader_h
