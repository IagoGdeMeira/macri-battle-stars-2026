#ifndef character_loader_h
#define character_loader_h

#include "AnimationLoader/AnimationLoader.h"
#include "CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "HitboxLoader/HitboxLoader.h"
#include "HurtboxLoader/HurtboxLoader.h"
#include "PushboxLoader/PushboxLoader.h"
#include "StateMachineLoader/StateMachineLoader.h"
#include "StateMachineRegistry/StateMachineRegistry.h"

#include "domain/components/AnimationComponent.h"
#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/DataParser/DataParser.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

#include <cstdint>
#include <memory>

class CharacterLoader
{
public:
    struct Config
    {
        DataParser& parser;
        CharacterDefinitionLoader& defLoader;
        AnimationLoader& animLoader;
        StateMachineLoader& fsmLoader;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
        HitboxLoader& hitboxLoader;
        HurtboxLoader& hurtboxLoader;
        PushboxLoader& pushboxLoader;
        StateMachineRegistry& stateMachineRegistry;
    };

    explicit CharacterLoader(Config&& cfg) :
        parser(cfg.parser),
        defLoader(cfg.defLoader),
        animLoader(cfg.animLoader),
        fsmLoader(cfg.fsmLoader),
        resourceManager(cfg.resourceManager),
        textureLoader(cfg.textureLoader),
        hitboxLoader(cfg.hitboxLoader),
        hurtboxLoader(cfg.hurtboxLoader),
        pushboxLoader(cfg.pushboxLoader),
        stateMachineRegistry(cfg.stateMachineRegistry) {}

    Entity create(World& world, const std::string& path) const;

private:
    DataParser& parser;
    CharacterDefinitionLoader& defLoader;
    AnimationLoader& animLoader;
    StateMachineLoader& fsmLoader;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    HitboxLoader& hitboxLoader;
    HurtboxLoader& hurtboxLoader;
    PushboxLoader& pushboxLoader;
    StateMachineRegistry& stateMachineRegistry;

    SpriteComponent buildSpriteComponent(const CharacterDefinition& def) const;
    std::shared_ptr<StateIdMapper> buildStateMapper(const CharacterDefinition& def) const;
    uint32_t registerStateMachine(const CharacterDefinition& def, const StateIdMapper& mapper) const;
    void addStateComponents(World& world, Entity entity, uint32_t machineId) const;
    AnimationControllerComponent buildAnimationController(const CharacterDefinition& def, const StateIdMapper& mapper) const;
    AnimationComponent buildInitialAnimation(Entity entity, World& world) const;

    void loadCollisionControllers(World& world, Entity entity, const CharacterDefinition& def) const;
};

#endif // character_loader_h
