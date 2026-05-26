#include "../../scenes/GameScene/GameScene.h"

#include "../../include/AirFrictionSystem/AirFrictionSystem.h"
#include "../../include/AnimationStateSystem/AnimationStateSystem.h"
#include "../../include/AnimationSystem/AnimationSystem.h"
#include "../../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../../include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"
#include "../../include/CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "../../include/ComboSystem/ComboSystem.h"
#include "../../include/ComponentRegistry/ComponentRegistry.h"
#include "../../include/DamageSystem/DamageSystem.h"
#include "../../include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"
#include "../../include/FaceOffSystem/FaceOffSystem.h"
#include "../../include/FrictionSystem/FrictionSystem.h"
#include "../../include/GravitySystem/GravitySystem.h"
#include "../../include/GroundDetectionSystem/GroundDetectionSystem.h"
#include "../../include/LocalToWorldSystem/LocalToWorldSystem.h"
#include "../../include/MovementSystem/MovementSystem.h"
#include "../../include/PlayerControlSystem/PlayerControlSystem.h"
#include "../../include/StateSystem/StateSystem.h"
#include "../../include/StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"
#include "../../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../../domain/components/AirFrictionComponent.h"
#include "../../../domain/components/AnalogInputComponent.h"
#include "../../../domain/components/GravityComponent.h"
#include "../../../domain/components/GroundedComponent.h"
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
    resourceManager(config.resourceManager),
    textureLoader(config.textureLoader)
{
    auto& world = this->world();
    auto& resources = this->resourceManager;
    auto& textures = this->textureLoader;
    this->entityFactory = std::make_unique<EntityFactory>(world, resources, textures);

    auto& systems = this->systemManager;
    auto& events = this->eventBus;

    auto& inputs = this->input;
    auto& trigger = this->triggerContext;
    systems.addSystem<InputSystem>(events, inputs);
    systems.addSystem<TriggerGenerationSystem>(events, trigger);
    systems.addSystem<InputBufferSystem>(events, inputs);
    systems.addSystem<ComboSystem>(events, this->combos);

    systems.addSystem<StateSystem>(events);

    systems.addSystem<CollisionClipPlayerSystem>(events, *this->entityFactory);
    systems.addSystem<PlayerControlSystem>(events, 300.0f, -500.0f);
    systems.addSystem<FaceOffSystem>(events);

    auto& map = this->mapData;
    systems.addSystem<GravitySystem>(map.gravity);
    systems.addSystem<AirFrictionSystem>(map.airFriction);
    systems.addSystem<MovementSystem>();
    systems.addSystem<LocalToWorldSystem>();

    systems.addSystem<AnimationStateSystem>(events);
    systems.addSystem<AnimationSystem>();

    systems.addSystem<CollisionDetectionSystem>();
    systems.addSystem<GroundDetectionSystem>(events);
    systems.addSystem<StaticPushboxResolutionSystem>(events);
    systems.addSystem<DynamicPushboxResolutionSystem>(events);
    systems.addSystem<FrictionSystem>(map.floorFriction);

    systems.addSystem<DamageSystem>(events);

    auto& cam = this->camera;
    systems.addSystem<CameraControllerSystem>(cam, this->window);

    this->worldDrawer = std::make_unique<WorldDrawer>(events, this->renderer, cam);
}

void GameScene::init() { this->prepareScene(); }

void GameScene::render()
{
    RenderContext ctx { this->world(), this->eventBus };
    this->worldDrawer->draw(ctx);
}

void GameScene::prepareScene()
{
    this->prepareComponents();
    this->prepareBackgroundLayers();
    this->prepareFloor();
    this->prepareWalls();
    this->preparePlayers();
}

void GameScene::prepareComponents() { ComponentRegistry::registerAll(this->world().components()); }

void GameScene::prepareBackgroundLayers()
{ for (const auto& layer : this->mapData.backgroundLayers) this->entityFactory->createBackgroundLayer(layer); }

void GameScene::prepareFloor()
{
    Position floorPos{this->mapData.floorWidth * 0.5f, this->mapData.floorY};
    Entity floorBody = this->entityFactory->createStaticBody(floorPos);
    this->entityFactory->addStaticCollider(floorBody,
        Rectangle{{0, 0}, this->mapData.floorWidth, this->mapData.floorHeight});
}

void GameScene::prepareWalls()
{
    auto& components = this->world().components();
    auto& entities = this->world().entities();

    for (const auto& wall : this->mapData.walls)
    {
        Entity w = entities.create();

        auto wallTransform = TransformComponent
        {
            wall.position.x + wall.size.width * 0.5f,
            wall.position.y + wall.size.height * 0.5f
        };
        components.add<TransformComponent>(w, wallTransform);
        components.add<RectangleColliderComponent>(w, RectangleColliderComponent{wall.size.width, wall.size.height});
    }
}

void GameScene::preparePlayers()
{ for (const auto& slot : this->playerSlots) this->preparePlayer(slot); }

void GameScene::preparePlayer(const PlayerSlot& slot)
{
    auto& world = this->world();
    auto& components = world.components();

    Entity entity = this->characterLoader.create(world, slot.characterDefPath);

    components.add<PlayerComponent>(entity, PlayerComponent{ slot.playerId });
    components.add<InputComponent>(entity, InputComponent{});
    components.add<InputBufferComponent>(entity, InputBufferComponent{});
    components.add<AnalogInputComponent>(entity, AnalogInputComponent{});
    components.add<TransformComponent>(entity, TransformComponent{});
    components.add<VelocityComponent>(entity, VelocityComponent{});
    components.add<GravityComponent>(entity, GravityComponent{});
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{});
    components.add<GroundedComponent>(entity, GroundedComponent{});

    float spawnX = 400.0f;
    for (const auto& sp : this->mapData.spawnPoints)
    { if (sp.playerId == slot.playerId) { spawnX = sp.x; break; } }

    auto& transform = components.get<TransformComponent>(entity);
    transform.x = spawnX;

    if (components.has<SpriteComponent>(entity))
    {
        const auto& sprite = components.get<SpriteComponent>(entity);
        transform.y = this->mapData.floorY - (sprite.size.height * 0.5f);
    }
    else transform.y = this->mapData.floorY - 32.0f;
    
    if (components.has<StateComponent>(entity))
    { components.get<StateComponent>(entity).current = StateId::Idle; }
}
