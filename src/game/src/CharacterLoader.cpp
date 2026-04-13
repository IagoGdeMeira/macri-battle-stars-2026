#include "../include/CharacterLoader/CharacterLoader.h"

#include "../../domain/components/AnimationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"

Entity CharacterLoader::create(World& world, const std::string& path) const
{
    auto def = defLoader.load(path);

    Entity entity = world.entities().create();
    auto& components = world.components();

    auto texture = resourceManager.load<Texture>(textureLoader, def.texturePath);

    SpriteComponent sprite;
    sprite.texture = texture;
    sprite.width = def.spriteWidth;
    sprite.height = def.spriteHeight;
    sprite.useSourceRect = false;

    components.add<SpriteComponent>(entity, std::move(sprite));

    auto fsm = fsmLoader.load(def.stateMachinePath);
    components.add<StateComponent>(entity, StateComponent{});

    AnimationControllerComponent controller;
    controller.animations = animLoader.load(def.animationsPath);

    components.add<AnimationControllerComponent>(entity, std::move(controller));
    components.add<AnimationComponent>(entity, AnimationComponent{});

    return entity;
}
