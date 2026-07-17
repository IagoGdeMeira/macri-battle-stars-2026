#include "../include/CharacterLoader/CharacterLoader.h"

#include "../include/StateIdMapper/StateIdMapper.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/CollisionClipDefinitionsComponent.h"
#include "../../domain/components/CollisionClipPlayerComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/StateMappingComponent.h"
#include "../../domain/components/StateMachineComponent.h"
#include "../../domain/utils/Logger/Logger.h"

#include <memory>

Entity CharacterLoader::create(World& world, const std::string& path) const
{
    auto def = this->defLoader.load(path);

    Entity entity = world.entities().create();
    auto& comp = world.components();

    SpriteComponent sprite;
    sprite.texturePath = def.texturePath;
    sprite.size.width = static_cast<float>(def.spriteSize.width);
    sprite.size.height = static_cast<float>(def.spriteSize.height);
    sprite.source.position = { 0.f, 0.f };
    sprite.source.size = sprite.size;
    sprite.useSourceRect = false;
    comp.add<SpriteComponent>(entity, std::move(sprite));

    auto mapper = std::make_shared<StateIdMapper>();
    for (const auto& customStateName : def.customStates) mapper->addCustomMapping(customStateName);

    auto fsm = this->fsmLoader.load(def.stateMachinePath, *mapper);
    comp.add<StateComponent>(entity, StateComponent{ StateId::Idle });
    comp.add<StateMappingComponent>(entity, StateMappingComponent{ mapper });
    comp.add<StateMachineComponent>(entity, StateMachineComponent{ std::move(fsm) });

    AnimationControllerComponent controller;
    controller.animations = this->animLoader.load(def.animationsPath, *mapper);
    controller.currentState = StateId::Idle;
    comp.add<AnimationControllerComponent>(entity, std::move(controller));

    if (!def.collisionsPath.empty())
    {
        auto clipMap = this->clipLoader.load(def.collisionsPath, *mapper);
        CollisionClipDefinitionsComponent clipDefs;

        for (auto& [state, clip] : clipMap) clipDefs.clips[state] = std::make_shared<CollisionClip>(std::move(clip));
        comp.add<CollisionClipDefinitionsComponent>(entity, std::move(clipDefs));
    }
    comp.add<CollisionClipPlayerComponent>(entity, CollisionClipPlayerComponent{});

    AnimationComponent anim;
    anim.currentState = StateId::Idle;
    anim.currentFrame = 0;
    anim.elapsedTime = 0.f;

    const auto& controllerRef = comp.get<AnimationControllerComponent>(entity);
    auto it = controllerRef.animations.right.find(StateId::Idle);
    if (it != controllerRef.animations.right.end()) anim.animation = it->second;
    else LOG_WARN("CharacterLoader: Idle animation not found for entity {}", entity.id);
    
    comp.add<AnimationComponent>(entity, std::move(anim));

    LOG_DEBUG("CharacterLoader: created entity {} with sprite texture {}", entity.id, def.texturePath);
    return entity;
}
