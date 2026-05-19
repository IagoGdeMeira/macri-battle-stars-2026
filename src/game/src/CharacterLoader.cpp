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

#include <memory>

CharacterLoader::CharacterLoader(Config&& config) :
    defLoader(config.defLoader),
    animLoader(config.animLoader),
    fsmLoader(config.fsmLoader),
    resourceManager(config.resourceManager),
    textureLoader(config.textureLoader),
    clipLoader(config.clipLoader) {}

Entity CharacterLoader::create(World& world, const std::string& path) const
{
    auto def = this->defLoader.load(path);

    Entity entity = world.entities().create();
    auto& components = world.components();

    auto texture = this->resourceManager.load<Texture>(this->textureLoader, def.texturePath);

    SpriteComponent sprite;
    sprite.texture = texture;
    sprite.size.width = static_cast<float>(def.spriteWidth);
    sprite.size.height = static_cast<float>(def.spriteHeight);
    sprite.source.position = { 0.f, 0.f };
    sprite.source.size = sprite.size;
    sprite.useSourceRect = false;
    components.add<SpriteComponent>(entity, std::move(sprite));

    auto mapper = std::make_shared<StateIdMapper>();
    for (const auto& customStateName : def.customStates) mapper->addCustomMapping(customStateName);

    auto fsm = this->fsmLoader.load(def.stateMachinePath, *mapper);
    components.add<StateComponent>(entity, StateComponent{});
    components.add<StateMappingComponent>(entity, StateMappingComponent{ mapper });
    components.add<StateMachineComponent>(entity, StateMachineComponent{ std::move(fsm) });

    AnimationControllerComponent controller;
    controller.animations = this->animLoader.load(def.animationsPath, *mapper);
    components.add<AnimationControllerComponent>(entity, std::move(controller));
    components.add<AnimationComponent>(entity, AnimationComponent{});

    if (!def.collisionsPath.empty())
    {
        auto clipMap = this->clipLoader.load(def.collisionsPath, *mapper);
        CollisionClipDefinitionsComponent clipDefs;

        for (auto& [state, clip] : clipMap)
        { clipDefs.clips[state] = std::make_shared<CollisionClip>(std::move(clip)); }

        components.add<CollisionClipDefinitionsComponent>(entity, std::move(clipDefs));
    }
    components.add<CollisionClipPlayerComponent>(entity, CollisionClipPlayerComponent{});

    return entity;
}
