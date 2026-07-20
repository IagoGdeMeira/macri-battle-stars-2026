#ifndef character_loader_h
#define character_loader_h

#include "AnimationLoader/AnimationLoader.h"
#include "CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "CollisionClipLoader/CollisionClipLoader.h"
#include "StateMachineLoader/StateMachineLoader.h"
#include "StateMachineRegistry/StateMachineRegistry.h"

#include "domain/include/World/World.h"
#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/SpriteComponent.h"

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

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
        StateMachineRegistry& stateMachineRegistry;
    };

    explicit CharacterLoader(Config&& cfg) :
        defLoader(cfg.defLoader),
        animLoader(cfg.animLoader),
        fsmLoader(cfg.fsmLoader),
        resourceManager(cfg.resourceManager),
        textureLoader(cfg.textureLoader),
        clipLoader(cfg.clipLoader),
        stateMachineRegistry(cfg.stateMachineRegistry) {}

    Entity create(World& world, const std::string& path) const;

private:
    CharacterDefinitionLoader& defLoader;
    AnimationLoader& animLoader;
    StateMachineLoader& fsmLoader;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    CollisionClipLoader& clipLoader;
    StateMachineRegistry& stateMachineRegistry;

    SpriteComponent buildSpriteComponent(const CharacterDefinition& def) const;
    std::shared_ptr<StateIdMapper> buildStateMapper(const CharacterDefinition& def) const;
    uint32_t registerStateMachine(const CharacterDefinition& def, const StateIdMapper& mapper) const;
    void addStateComponents(World& world, Entity entity, uint32_t machineId) const;
    AnimationControllerComponent buildAnimationController(const CharacterDefinition& def, const StateIdMapper& mapper) const;
    void addCollisionComponents(World& world, Entity entity, const CharacterDefinition& def, const StateIdMapper& mapper) const;
    AnimationComponent buildInitialAnimation(Entity entity, World& world) const;
};

#endif // character_loader_h
