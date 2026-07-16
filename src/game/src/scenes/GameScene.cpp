#include "../scenes/GameScene.h"

#include "../collision_detections/CircleCircleCollisionDetection.h"
#include "../collision_detections/RectCircleCollisionDetection.h"
#include "../collision_detections/RectRectCollisionDetection.h"
#include "../include/AirFrictionSystem/AirFrictionSystem.h"
#include "../include/AnimationLoader/AnimationLoader.h"
#include "../include/AnimationStateSystem/AnimationStateSystem.h"
#include "../include/AnimationSystem/AnimationSystem.h"
#include "../include/AttackSystem/AttackSystem.h"
#include "../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../include/CollisionClipLoader/CollisionClipLoader.h"
#include "../include/CollisionClipPlayerSystem/CollisionClipPlayerSystem.h"
#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "../include/ComboLoader/ComboLoader.h"
#include "../include/ComboSystem/ComboSystem.h"
#include "../include/ComponentRegistry/ComponentRegistry.h"
#include "../include/CrouchSystem/CrouchSystem.h"
#include "../include/DamageSystem/DamageSystem.h"
#include "../include/DirectionTriggerSystem/DirectionTriggerSystem.h"
#include "../include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"
#include "../include/FaceOffSystem/FaceOffSystem.h"
#include "../include/FallTriggerSystem/FallTriggerSystem.h"
#include "../include/FrictionSystem/FrictionSystem.h"
#include "../include/GravitySystem/GravitySystem.h"
#include "../include/GroundDetectionSystem/GroundDetectionSystem.h"
#include "../include/HorizontalMovementSystem/HorizontalMovementSystem.h"
#include "../include/JumpSystem/JumpSystem.h"
#include "../include/LocalToWorldSystem/LocalToWorldSystem.h"
#include "../include/MapLoader/MapLoader.h"
#include "../include/MovementSystem/MovementSystem.h"
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
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/utils/Logger/Logger.h"

#include "../../engine/include/InputBindingLoader/InputBindingLoader.h"
#include "../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/InputManager/InputManager.h"
#include "../../engine/include/InputSystem/InputSystem.h"
#include "../../engine/include/RenderContext/RenderContext.h"

GameScene::GameScene(Config&& cfg) :
    Scene(*cfg.eventBus),
    eventBus(*cfg.eventBus),
    sceneManager(*cfg.sceneManager),
    renderer(*cfg.renderer),
    window(*cfg.window),
    parser(*cfg.parser),
    resourceManager(*cfg.resourceManager),
    textureLoader(*cfg.textureLoader),
    settings(*cfg.settings),
    engine(*cfg.engine),
    fontFactory(*cfg.fontFactory),
    textureFactory(*cfg.textureFactory),
    platformFactory(*cfg.platformFactory),
    playerSlots(std::move(cfg.playerSlots)),
    mapPath(cfg.mapPath),
    mapRoot(Entity{0})
{
    this->loadInputContext(cfg.inputBindingsPath);

    if (!cfg.combosPath.empty()) this->loadCombos(cfg.combosPath);
    if (!cfg.triggersPath.empty()) this->loadTriggerBindings(cfg.triggersPath);

    if (cfg.platformFactory) this->setupInputAdapters();

    this->createCharacterLoader();
    this->camera = std::make_unique<Camera2D>();
    this->entityFactory = std::make_unique<EntityFactory>(this->world(), this->resourceManager, this->textureLoader);
}

void GameScene::init()
{
    this->prepareScene();

    auto& systems = this->systemManager;
    auto& entities = this->world().entities();
    auto& comp = this->world().components();
    auto& events = this->eventBus;

    systems.addSystem<InputSystem>(events, *this->inputContext);
    systems.addSystem<TriggerGenerationSystem>(events, this->triggerContext);
    systems.addSystem<InputBufferSystem>(events, *this->inputContext);
    systems.addSystem<ComboSystem>(events, this->combos);

    systems.addSystem<DirectionTriggerSystem>(events);
    systems.addSystem<CrouchSystem>(events);
    systems.addSystem<AttackSystem>(events, InputAction::Punch, TriggerId::Punched);
    systems.addSystem<AttackSystem>(events, InputAction::Kick, TriggerId::Kicked);
    systems.addSystem<AttackSystem>(events, InputAction::Defend, TriggerId::Blocked);
    systems.addSystem<FallTriggerSystem>(events);

    systems.addSystem<StateSystem>(events);

    systems.addSystem<HorizontalMovementSystem>(300.f);
    systems.addSystem<JumpSystem>(events, -500.f);
    systems.addSystem<FaceOffSystem>(events);

    if (!entities.isAlive(this->mapRoot)) throw std::runtime_error("GameScene::init() - mapRoot is not alive");
    auto& mapComp = comp.get<MapComponent>(this->mapRoot);

    // systems.addSystem<GravitySystem>(mapComp.gravity);
    systems.addSystem<AirFrictionSystem>(mapComp.airFriction);
    systems.addSystem<MovementSystem>();
    systems.addSystem<LocalToWorldSystem>();

    systems.addSystem<AnimationStateSystem>(events);
    systems.addSystem<AnimationSystem>();

    auto& collisionDetectionSystem = systems.addSystem<CollisionDetectionSystem>(1000.f);
    collisionDetectionSystem.addDetector(std::make_unique<RectRectCollisionDetection>());
    collisionDetectionSystem.addDetector(std::make_unique<RectCircleCollisionDetection>());
    collisionDetectionSystem.addDetector(std::make_unique<CircleCircleCollisionDetection>());

    systems.addSystem<GroundDetectionSystem>(events);
    systems.addSystem<StaticPushboxResolutionSystem>(events);
    systems.addSystem<DynamicPushboxResolutionSystem>(events);
    systems.addSystem<FrictionSystem>(mapComp.floorFriction);

    systems.addSystem<DamageSystem>(events);
    systems.addSystem<CollisionClipPlayerSystem>(events, *this->entityFactory);
    LOG_DEBUG("GameScene: adding CameraControllerSystem");
    systems.addSystem<CameraControllerSystem>(CameraControllerSystem::Config{
        .camera         = *this->camera,
        .window         = this->window,
        .minZoom        = 1.f,
        .maxZoom        = 10.f,
        .verticalOffset = -70.f,
        .bounds         = mapComp.worldBounds
    });
    LOG_DEBUG("GameScene: CameraControllerSystem added");
    LOG_DEBUG("GameScene: total systems = {}", this->systems().size());

    this->worldDrawer = std::make_unique<WorldDrawer>(events, this->renderer, *this->camera, this->settings);
}

void GameScene::loadInputContext(const std::string& path)
{
    InputBindingLoader loader(this->parser);
    this->inputContext = std::make_unique<InputContext>(loader.load(path));
}

void GameScene::setupInputAdapters()
{
    auto& inputManager = this->engine.input();

    inputManager.setProvider(this->platformFactory.createEventProvider());
    inputManager.addAdapter(this->platformFactory.createKeyboardAdapter(this->eventBus, *this->inputContext));

    auto gamepadAdapters = this->platformFactory.createGamepadAdapters(this->eventBus, 1);
    for (auto& adapter : gamepadAdapters) inputManager.addAdapter(std::move(adapter));

    inputManager.addAdapter(this->platformFactory.createSystemAdapter(this->eventBus));
}

void GameScene::loadMap(const std::string& path)
{
    MapLoader mapLoader(this->parser, *this->entityFactory);
    this->mapRoot = mapLoader.load(this->world(), path);
}

void GameScene::loadCombos(const std::string& path) { this->combos = ComboLoader(this->parser).load(path); }

void GameScene::loadTriggerBindings(const std::string& path) { this->triggerContext = TriggerBindingLoader(this->parser).load(path); }

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
    this->loadMap(this->mapPath);
    this->preparePlayers();
}

void GameScene::prepareComponents() { ComponentRegistry::registerAll(this->world().components()); }

void GameScene::preparePlayers() { for (const auto& slot : this->playerSlots) this->preparePlayer(slot); }

void GameScene::preparePlayer(const PlayerSlot& slot)
{
    auto& world = this->world();
    auto& comp = world.components();

    Entity entity = this->characterLoader->create(world, slot.characterDefPath);

    comp.add<PlayerComponent>(entity, PlayerComponent{ slot.playerId });
    comp.add<InputComponent>(entity, InputComponent{});
    comp.add<InputBufferComponent>(entity, InputBufferComponent{});
    comp.add<AnalogInputComponent>(entity, AnalogInputComponent{});
    comp.add<TransformComponent>(entity, TransformComponent{});
    comp.add<VelocityComponent>(entity, VelocityComponent{});
    comp.add<GravityComponent>(entity, GravityComponent{});
    comp.add<AirFrictionComponent>(entity, AirFrictionComponent{});
    comp.add<GroundedComponent>(entity, GroundedComponent{});
    comp.add<RenderComponent>(entity, RenderComponent{ 0, 10 });
    comp.add<OrientationComponent>(entity, OrientationComponent{ Orientation::Right });

    LOG_DEBUG("GameScene: player entity {} state.current = {}",
        entity.id, comp.get<StateComponent>(entity).current.value());

    const auto& mapComp = comp.get<MapComponent>(this->mapRoot);

    float spawnX = 400.f;
    for (const auto& sp : mapComp.spawnPoints) if (sp.playerId == slot.playerId) { spawnX = sp.x; break; }

    auto& transform = comp.get<TransformComponent>(entity);
    transform.position.x = spawnX;

    if (comp.has<SpriteComponent>(entity))
    {
        const auto& sprite = comp.get<SpriteComponent>(entity);
        transform.position.y = mapComp.floorY - (sprite.size.height * 0.5f);
    }
    else transform.position.y = mapComp.floorY - 32.f;

    if (comp.has<StateComponent>(entity)) comp.get<StateComponent>(entity).current = StateId::Idle;
    LOG_DEBUG("preparePlayer: entity {} final position = ({}, {})", entity.id, transform.position.x, transform.position.y);
}
