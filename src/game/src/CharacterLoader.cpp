#include "CharacterLoader/CharacterLoader.h"

#include "StateIdMapper/StateIdMapper.h"

#include "domain/components/CollisionClipDefinitionsComponent.h"
#include "domain/components/CollisionClipPlayerComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/StateMachineComponent.h"
#include "domain/utils/Logger/Logger.h"

#include <memory>

Entity CharacterLoader::create(World& world, const std::string& path) const
{
    auto def = this->defLoader.load(path);
    Entity entity = world.entities().create();
    auto& comp = world.components();

    comp.add<SpriteComponent>(entity, this->buildSpriteComponent(def));

    auto mapper = this->buildStateMapper(def);
    uint32_t machineId = this->registerStateMachine(def, *mapper);

    this->addStateComponents(world, entity, machineId);

    comp.add<AnimationControllerComponent>(entity, this->buildAnimationController(def, *mapper));

    if (!def.collisionsPath.empty()) this->addCollisionComponents(world, entity, def, *mapper);
    
    comp.add<CollisionClipPlayerComponent>(entity, CollisionClipPlayerComponent{});
    comp.add<AnimationComponent>(entity, this->buildInitialAnimation(entity, world));

    LOG_DEBUG("CharacterLoader: created entity {} with sprite texture {}", entity.id, def.texturePath);
    return entity;
}

SpriteComponent CharacterLoader::buildSpriteComponent(const CharacterDefinition& def) const
{
    SpriteComponent sprite;
    sprite.texturePath = def.texturePath;
    sprite.size.width = static_cast<float>(def.spriteSize.width);
    sprite.size.height = static_cast<float>(def.spriteSize.height);
    sprite.source.position = {0.f, 0.f};
    sprite.source.size = sprite.size;
    sprite.useSourceRect = false;
    return sprite;
}

std::shared_ptr<StateIdMapper> CharacterLoader::buildStateMapper(const CharacterDefinition& def) const
{
    auto mapper = std::make_shared<StateIdMapper>();
    for (const auto& customStateName : def.customStates) mapper->addCustomMapping(customStateName);
    return mapper;
}

uint32_t CharacterLoader::registerStateMachine(const CharacterDefinition& def, const StateIdMapper& mapper) const
{
    StateMachine fsm = this->fsmLoader.load(def.stateMachinePath, mapper);
    return this->stateMachineRegistry.registerMachine(std::move(fsm));
}

void CharacterLoader::addStateComponents(World& world, Entity entity, uint32_t machineId) const
{
    auto& comp = world.components();
    comp.add<StateComponent>(entity, StateComponent{ StateId::Idle });
    comp.add<StateMachineComponent>(entity, StateMachineComponent{ machineId });
}

AnimationControllerComponent CharacterLoader::buildAnimationController(const CharacterDefinition& def, const StateIdMapper& mapper) const
{
    AnimationControllerComponent controller;
    controller.animations = this->animLoader.load(def.animationsPath, mapper);
    controller.currentState = StateId::Idle;
    return controller;
}

void CharacterLoader::addCollisionComponents(World& world, Entity entity, const CharacterDefinition& def, const StateIdMapper& mapper) const
{
    auto clipMap = this->clipLoader.load(def.collisionsPath, mapper);
    CollisionClipDefinitionsComponent clipDefs;
    for (auto& [state, clip] : clipMap) clipDefs.clips[state] = std::make_shared<CollisionClip>(std::move(clip));
    
    world.components().add<CollisionClipDefinitionsComponent>(entity, std::move(clipDefs));
}

AnimationComponent CharacterLoader::buildInitialAnimation(Entity entity, World& world) const
{
    auto& comp = world.components();
    AnimationComponent anim;
    anim.currentState = StateId::Idle;
    anim.currentFrame = 0;
    anim.elapsedTime = 0.f;

    const auto& controllerRef = comp.get<AnimationControllerComponent>(entity);
    auto it = controllerRef.animations.right.find(StateId::Idle);
    if (it != controllerRef.animations.right.end()) anim.animation = it->second;
    else LOG_WARN("CharacterLoader: Idle animation not found for entity {}", entity.id);
    return anim;
}
