#ifndef character_loader_h
#define character_loader_h

#include "../AnimationLoader/AnimationLoader.h"
#include "../CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../CollisionClipLoader/CollisionClipLoader.h"
#include "../StateMachineLoader/StateMachineLoader.h"

#include "../../domain/include/World/World.h"

#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"

class CharacterLoader
{
public:
    struct Config
    {
        CharacterDefinitionLoader& defLoader;
        AnimationLoader& animLoader;
        StateMachineLoader& fsmLoader;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
        CollisionClipLoader& clipLoader;
    };

    explicit CharacterLoader(Config&& config);

    Entity create(World& world, const std::string& path) const;

private:
    CharacterDefinitionLoader& defLoader;
    AnimationLoader& animLoader;
    StateMachineLoader& fsmLoader;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    CollisionClipLoader& clipLoader;
};

#endif // character_loader_h
