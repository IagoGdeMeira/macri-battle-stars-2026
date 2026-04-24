#include "../../scenes/GameScene/GameScene.h"

#include "../../include/AnimationStateSystem/AnimationStateSystem.h"
#include "../../include/AnimationSystem/AnimationSystem.h"
#include "../../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../../include/CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "../../include/ComboSystem/ComboSystem.h"
#include "../../include/ComponentRegistry/ComponentRegistry.h"
#include "../../include/DamageSystem/DamageSystem.h"
#include "../../include/LocalToWorldSystem/LocalToWorldSystem.h"
#include "../../include/RenderSystem/RenderSystem.h"
#include "../../include/StateSystem/StateSystem.h"
#include "../../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../../domain/components/InputBufferComponent.h"
#include "../../../domain/components/InputComponent.h"
#include "../../../domain/components/ParallaxComponent.h"
#include "../../../domain/components/PlayerComponent.h"
#include "../../../domain/components/RectangleColliderComponent.h"
#include "../../../domain/components/RenderComponent.h"
#include "../../../domain/components/SpriteComponent.h"
#include "../../../domain/components/StateComponent.h"
#include "../../../domain/components/StateMachineComponent.h"
#include "../../../domain/components/TransformComponent.h"
#include "../../../domain/components/VelocityComponent.h"

#include "../../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../../engine/include/InputSystem/InputSystem.h"
#include "../../../engine/include/MovementSystem/MovementSystem.h"
#include "../../../engine/include/RenderContext/RenderContext.h"

#include <utility>

GameScene::GameScene(Config&& config) :
    Scene(config.eventBus),
    input(config.input),
    triggerContext(std::move(config.triggerContext)),
    camera(config.camera),
    window(config.window),
    combos(std::move(config.combos)),
    characterLoader(config.characterLoader),
    playerSlots(std::move(config.playerSlots)),
    renderer(config.renderer),
    mapData(std::move(config.mapData)),
    resourceManager(config.resourceManager)
{
    systemManager.addSystem<InputSystem>(config.eventBus, this->input);
    systemManager.addSystem<TriggerGenerationSystem>(config.eventBus, this->triggerContext);
    systemManager.addSystem<InputBufferSystem>(config.eventBus, this->input);
    systemManager.addSystem<ComboSystem>(config.eventBus, this->combos);

    systemManager.addSystem<StateSystem>(config.eventBus);

    systemManager.addSystem<MovementSystem>();
    systemManager.addSystem<LocalToWorldSystem>();

    systemManager.addSystem<AnimationStateSystem>(config.eventBus);
    systemManager.addSystem<AnimationSystem>();

    systemManager.addSystem<CollisionDetectionSystem>();
    systemManager.addSystem<DamageSystem>(config.eventBus);

    systemManager.addSystem<CameraControllerSystem>(this->camera, this->window);

    this->renderSystem = std::make_unique<RenderSystem>(
        config.eventBus, config.renderer, config.camera);
}

void GameScene::init()
{
    auto& world = this->world();
    auto& components = this->world().components();
    auto& entities = this->world().entities();

    ComponentRegistry::registerAll(components);

    // Create background entities based on map data
    for (const auto& layer : this->mapData.backgroundLayers)
    {
        Entity bg = entities.create();

        auto bgParallax = ParallaxComponent { layer.parallaxFactorX, layer.parallaxFactorY };
        components.add<ParallaxComponent>(bg, bgParallax);

        components.add<RenderComponent>(bg, RenderComponent{ 0, layer.zIndex });
    }

    // Create floor entity with collider based on map data
    Entity floor = entities.create();
    auto floorTransform = TransformComponent { this->mapData.floorWidth * 0.5f, this->mapData.floorY };
    components.add<TransformComponent>(floor, floorTransform);
    
    auto floorCollider = RectangleColliderComponent { this->mapData.floorWidth, this->mapData.floorHeight };
    components.add<RectangleColliderComponent>(floor, floorCollider);

    // Create wall entities based on map data
    for (const auto& wall : this->mapData.walls)
    {
        Entity w = entities.create();

        auto wallTransform = TransformComponent
        {
            wall.position.x + wall.width * 0.5f,
            wall.position.y + wall.height * 0.5f
        };
        components.add<TransformComponent>(w, wallTransform);

        components.add<RectangleColliderComponent>(w, RectangleColliderComponent{wall.width, wall.height});
    }

    // Create player entities based on player slots and map spawn points
    for (const auto& slot : this->playerSlots)
    {
        Entity entity = this->characterLoader.create(world, slot.characterDefPath);

        components.add<PlayerComponent>(entity, PlayerComponent{slot.playerId});
        components.add<InputComponent>(entity, InputComponent{});
        components.add<InputBufferComponent>(entity, InputBufferComponent{});
        components.add<TransformComponent>(entity, TransformComponent{});
        components.add<VelocityComponent>(entity, VelocityComponent{});

        float spawnX = 400.0f;
        for (const auto& sp : this->mapData.spawnPoints) if (sp.playerId == slot.playerId)
        { spawnX = sp.x; break; }

        auto& transform = components.get<TransformComponent>(entity);
        transform.x = spawnX;

        if (components.has<SpriteComponent>(entity))
        {
            const auto& sprite = components.get<SpriteComponent>(entity);
            transform.y = this->mapData.floorY - (sprite.height * 0.5f);
        }
        else transform.y = this->mapData.floorY - 32.0f;
        

        if (components.has<StateComponent>(entity))
        { components.get<StateComponent>(entity).current = StateId::Idle; }
    }
}

void GameScene::render()
{
    RenderContext ctx { this->world(), this->eventBus };
    this->renderSystem->draw(ctx);
}
