#include "../scenes/GameScene.h"

#include "../include/AirFrictionSystem/AirFrictionSystem.h"
#include "../include/AnimationLoader/AnimationLoader.h"
#include "../include/AnimationStateSystem/AnimationStateSystem.h"
#include "../include/AnimationSystem/AnimationSystem.h"
#include "../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../include/CollisionClipLoader/CollisionClipLoader.h"
#include "../include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"
#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "../include/ComboLoader/ComboLoader.h"
#include "../include/ComboSystem/ComboSystem.h"
#include "../include/ComponentRegistry/ComponentRegistry.h"
#include "../include/DamageSystem/DamageSystem.h"
#include "../include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"
#include "../include/FaceOffSystem/FaceOffSystem.h"
#include "../include/FrictionSystem/FrictionSystem.h"
#include "../include/GravitySystem/GravitySystem.h"
#include "../include/GroundDetectionSystem/GroundDetectionSystem.h"
#include "../include/LocalToWorldSystem/LocalToWorldSystem.h"
#include "../include/MapLoader/MapLoader.h"
#include "../include/MovementSystem/MovementSystem.h"
#include "../include/PlayerControlSystem/PlayerControlSystem.h"
#include "../include/StateMachineLoader/StateMachineLoader.h"
#include "../include/StateSystem/StateSystem.h"
#include "../include/StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"
#include "../include/TriggerBindingLoader/TriggerBindingLoader.h"
#include "../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../domain/components/AirFrictionComponent.h"
#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/GravityComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"

#include "../../engine/include/InputBindingLoader/InputBindingLoader.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/RenderContext/RenderContext.h"
#include "../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../engine/include/InputSystem/InputSystem.h"

GameScene::GameScene(Config&& config) : 
    Scene(*config.eventBus),
    eventBus(*config.eventBus),
    sceneManager(*config.sceneManager),
    renderer(*config.renderer),
    window(*config.window),
    parser(*config.parser),
    resourceManager(*config.resourceManager),
    textureLoader(*config.textureLoader),
    settings(*config.settings),
    engine(*config.engine),
    fontFactory(*config.fontFactory),
    textureFactory(*config.textureFactory),
    playerSlots(std::move(config.playerSlots))
{
    this->loadInputContext(config.inputBindingsPath);
    this->loadMap(config.mapPath);

    if (!config.combosPath.empty()) this->loadCombos(config.combosPath);
    if (!config.triggersPath.empty()) this->loadTriggerBindings(config.triggersPath);

    this->createCharacterLoader();
    this->camera = std::make_unique<Camera2D>();
    this->entityFactory = std::make_unique<EntityFactory>(this->world(), this->resourceManager, this->textureLoader);
}

void GameScene::init()
{
    this->prepareScene();

    auto& systems = this->systemManager;
    auto& events = this->eventBus;

    systems.addSystem<InputSystem>(events, *this->inputContext);
    systems.addSystem<TriggerGenerationSystem>(events, this->triggerContext);
    systems.addSystem<InputBufferSystem>(events, *this->inputContext);
    systems.addSystem<ComboSystem>(events, this->combos);
    systems.addSystem<StateSystem>(events);
    systems.addSystem<CollisionClipPlayerSystem>(events, *this->entityFactory);
    systems.addSystem<PlayerControlSystem>(events, 300.f, -500.f);
    systems.addSystem<FaceOffSystem>(events);
    systems.addSystem<GravitySystem>(this->mapData.gravity);
    systems.addSystem<AirFrictionSystem>(this->mapData.airFriction);
    systems.addSystem<MovementSystem>();
    systems.addSystem<LocalToWorldSystem>();
    systems.addSystem<AnimationStateSystem>(events);
    systems.addSystem<AnimationSystem>();
    systems.addSystem<CollisionDetectionSystem>();
    systems.addSystem<GroundDetectionSystem>(events);
    systems.addSystem<StaticPushboxResolutionSystem>(events);
    systems.addSystem<DynamicPushboxResolutionSystem>(events);
    systems.addSystem<FrictionSystem>(this->mapData.floorFriction);
    systems.addSystem<DamageSystem>(events);
    systems.addSystem<CameraControllerSystem>(*this->camera, this->window);

    this->worldDrawer = std::make_unique<WorldDrawer>(events, this->renderer, *this->camera, this->settings);
}

void GameScene::loadInputContext(const std::string& path)
{
    InputBindingLoader loader(this->parser);
    this->inputContext = std::make_unique<InputContext>(loader.load(path));
}

void GameScene::loadMap(const std::string& path)
{
    MapLoader loader(this->parser);
    this->mapData = loader.load(path);
}

void GameScene::loadCombos(const std::string& path)
{
    ComboLoader loader(this->parser);
    this->combos = loader.load(path);
}

void GameScene::loadTriggerBindings(const std::string& path)
{
    TriggerBindingLoader loader(this->parser);
    this->triggerContext = loader.load(path);
}

void GameScene::createCharacterLoader()
{
    this->charDefLoader = std::make_unique<CharacterDefinitionLoader>(this->parser);
    this->animLoader    = std::make_unique<AnimationLoader>(this->parser);
    this->fsmLoader     = std::make_unique<StateMachineLoader>(this->parser);
    this->clipLoader    = std::make_unique<CollisionClipLoader>(this->parser);

    this->characterLoader = std::make_unique<CharacterLoader>(CharacterLoader::Config{
        .defLoader          = *this->charDefLoader,
        .animLoader         = *this->animLoader,
        .fsmLoader          = *this->fsmLoader,
        .resourceManager    = this->resourceManager,
        .textureLoader      = this->textureLoader,
        .clipLoader         = *this->clipLoader
    });
}

void GameScene::render()
{
    RenderContext ctx{ this->world(), this->eventBus };
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
    Position floorPos{ this->mapData.floorWidth * 0.5f, this->mapData.floorY };
    Entity floorBody = this->entityFactory->createStaticBody(floorPos);
    this->entityFactory->addStaticCollider(floorBody, Rectangle{{0, 0}, this->mapData.floorWidth, this->mapData.floorHeight });
}

void GameScene::prepareWalls()
{
    auto& components = this->world().components();
    auto& entities = this->world().entities();

    for (const auto& wall : this->mapData.walls)
    {
        Entity w = entities.create();
        TransformComponent wallTransform{
            wall.position.x + wall.size.width * 0.5f,
            wall.position.y + wall.size.height * 0.5f
        };
        components.add<TransformComponent>(w, wallTransform);
        components.add<RectangleColliderComponent>(w, RectangleColliderComponent{ wall.size.width, wall.size.height });
    }
}

void GameScene::preparePlayers()
{ for (const auto& slot : this->playerSlots) this->preparePlayer(slot); }

void GameScene::preparePlayer(const PlayerSlot& slot)
{
    auto& world = this->world();
    auto& components = world.components();

    Entity entity = this->characterLoader->create(world, slot.characterDefPath);

    components.add<PlayerComponent>(entity, PlayerComponent{ slot.playerId });
    components.add<InputComponent>(entity, InputComponent{});
    components.add<InputBufferComponent>(entity, InputBufferComponent{});
    components.add<AnalogInputComponent>(entity, AnalogInputComponent{});
    components.add<TransformComponent>(entity, TransformComponent{});
    components.add<VelocityComponent>(entity, VelocityComponent{});
    components.add<GravityComponent>(entity, GravityComponent{});
    components.add<AirFrictionComponent>(entity, AirFrictionComponent{});
    components.add<GroundedComponent>(entity, GroundedComponent{});

    float spawnX = 400.f;
    for (const auto& sp : this->mapData.spawnPoints)
    {
        if (sp.playerId != slot.playerId) continue;
        spawnX = sp.x; break;
    }

    auto& transform = components.get<TransformComponent>(entity);
    transform.position.x = spawnX;

    if (components.has<SpriteComponent>(entity))
    {
        const auto& sprite = components.get<SpriteComponent>(entity);
        transform.position.y = this->mapData.floorY - (sprite.size.height * 0.5f);
    }
    else transform.position.y = this->mapData.floorY - 32.f;

    if (components.has<StateComponent>(entity)) components.get<StateComponent>(entity).current = StateId::Idle;
}
