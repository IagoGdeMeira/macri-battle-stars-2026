#include "CharacterLoader/CharacterLoader.h"

#include "StateIdMapper/StateIdMapper.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/StateMachineComponent.h"
#include "domain/utils/Logger/Logger.h"

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
    
    this->loadCollisionControllers(world, entity, def);

    comp.add<AnimationComponent>(entity, this->buildInitialAnimation(entity, world));
    comp.add<JumpComponent>(entity, JumpComponent{
        .force              = def.jump.force,
        .maxTime            = def.jump.maxTime,
        .timer              = 0.f,
        .gravityScaleAsc    = def.jump.gravityScaleAsc,
        .gravityScaleDesc   = def.jump.gravityScaleDesc,
        .fastFallMultiplier = def.jump.fastFallMultiplier
    });

    LOG_DEBUG("JumpComponent loaded: force={}, maxTime={}, asc={}, desc={}",
        def.jump.force, def.jump.maxTime, def.jump.gravityScaleAsc, def.jump.gravityScaleDesc);

    return entity;
}

SpriteComponent CharacterLoader::buildSpriteComponent(const CharacterDefinition& def) const
{
    SpriteComponent sprite;
    sprite.texturePath      = def.texturePath;
    sprite.size.width       = def.spriteSize.width;
    sprite.size.height      = def.spriteSize.height;
    sprite.source.position  = Position{0.f, 0.f};
    sprite.source.size      = sprite.size;
    sprite.useSourceRect    = false;
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
    controller.animations = this->animLoader.loadFromIndex(def.animationsPath, mapper);
    controller.currentState = StateId::Idle;
    return controller;
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
    return anim;
}

void CharacterLoader::loadCollisionControllers(World& world, Entity entity, const CharacterDefinition& def) const
{
    if (def.collisionsPath.empty()) return;

    auto root = this->parser.parse(def.collisionsPath);
    auto& comp = world.components();

    HitboxControllerMapComponent hitboxMapComp;
    HurtboxControllerMapComponent hurtboxMapComp;
    PushboxControllerMapComponent pushboxMapComp;

    for (auto& entry : root->getArray("states"))
    {
        std::string stateName = entry->getString("name");
        std::string stateFilePath = entry->getString("path");
        StateId stateId = StateId::fromBaseName(stateName);

        auto stateRoot = this->parser.parse(stateFilePath);

        hitboxMapComp.map[stateId]   = this->hitboxLoader.loadSingleState(*stateRoot, entity);
        hurtboxMapComp.map[stateId]  = this->hurtboxLoader.loadSingleState(*stateRoot, entity);
        pushboxMapComp.map[stateId]  = this->pushboxLoader.loadSingleState(*stateRoot, entity);
    }

    comp.add<HitboxControllerMapComponent>(entity, std::move(hitboxMapComp));
    comp.add<HurtboxControllerMapComponent>(entity, std::move(hurtboxMapComp));
    comp.add<PushboxControllerMapComponent>(entity, std::move(pushboxMapComp));
}
