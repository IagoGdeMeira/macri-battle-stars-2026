#include "GameScene.h"

#include "AirFrictionSystem/AirFrictionSystem.h"
#include "AnimationLoader/AnimationLoader.h"
#include "AnimationStateSystem/AnimationStateSystem.h"
#include "AnimationSystem/AnimationSystem.h"
#include "AttackSystem/AttackSystem.h"
#include "CameraControllerSystem/CameraControllerSystem.h"
#include "CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "CircleCircleCollisionDetection.h"
#include "CollisionControllerSystem/CollisionControllerSystem.h"
#include "CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "ComboLoader/ComboLoader.h"
#include "ComboSystem/ComboSystem.h"
#include "ComponentRegistry/ComponentRegistry.h"
#include "CrouchSystem/CrouchSystem.h"
#include "DamageSystem/DamageSystem.h"
#include "DirectionTriggerSystem/DirectionTriggerSystem.h"
#include "DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"
#include "FaceOffSystem/FaceOffSystem.h"
#include "FallTriggerSystem/FallTriggerSystem.h"
#include "FrictionSystem/FrictionSystem.h"
#include "GravitySystem/GravitySystem.h"
#include "GroundDetectionSystem/GroundDetectionSystem.h"
#include "HitboxCollisionController.h"
#include "HitboxControllerSystem/HitboxControllerSystem.h"
#include "HitboxLoader/HitboxLoader.h"
#include "HorizontalMovementSystem/HorizontalMovementSystem.h"
#include "HurtboxCollisionController.h"
#include "HurtboxControllerSystem/HurtboxControllerSystem.h"
#include "HurtboxLoader/HurtboxLoader.h"
#include "JumpSystem/JumpSystem.h"
#include "LocalToWorldSystem/LocalToWorldSystem.h"
#include "MapLoader/MapLoader.h"
#include "MovementSystem/MovementSystem.h"
#include "PushboxCollisionController.h"
#include "PushboxControllerSystem/PushboxControllerSystem.h"
#include "PushboxLoader/PushboxLoader.h"
#include "RectCircleCollisionDetection.h"
#include "RectRectCollisionDetection.h"
#include "StateMachineLoader/StateMachineLoader.h"
#include "StateSystem/StateSystem.h"
#include "StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"
#include "TriggerBindingLoader/TriggerBindingLoader.h"
#include "TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "domain/components/AirFrictionComponent.h"
#include "domain/components/AnalogInputComponent.h"
#include "domain/components/GravityComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/InputBufferComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/StateComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/include/InputBindingLoader/InputBindingLoader.h"
#include "engine/include/InputBufferSystem/InputBufferSystem.h"
#include "engine/include/InputManager/InputManager.h"
#include "engine/include/InputSystem/InputSystem.h"
#include "engine/value_objects/InputContext/InputContext.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

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

    this->charDefLoader = std::make_unique<CharacterDefinitionLoader>(this->parser);
    this->animLoader = std::make_unique<AnimationLoader>(this->parser);
    this->fsmLoader = std::make_unique<StateMachineLoader>(this->parser);
    this->stateMachineRegistry = std::make_unique<StateMachineRegistry>();

    this->entityFactory = std::make_unique<EntityFactory>(EntityFactory::Config{
        this->world(), this->resourceManager, this->textureLoader, *this->animLoader });

    this->hitboxLoader = std::make_unique<HitboxLoader>(this->parser, *this->entityFactory);
    this->hurtboxLoader = std::make_unique<HurtboxLoader>(this->parser, *this->entityFactory);
    this->pushboxLoader = std::make_unique<PushboxLoader>(this->parser, *this->entityFactory);

    this->characterLoader = std::make_unique<CharacterLoader>(CharacterLoader::Config{
        .parser                 = this->parser,
        .defLoader              = *this->charDefLoader,
        .animLoader             = *this->animLoader,
        .fsmLoader              = *this->fsmLoader,
        .resourceManager        = this->resourceManager,
        .textureLoader          = this->textureLoader,
        .hitboxLoader           = *this->hitboxLoader,
        .hurtboxLoader          = *this->hurtboxLoader,
        .pushboxLoader          = *this->pushboxLoader,
        .stateMachineRegistry   = *this->stateMachineRegistry
    });

    this->camera = std::make_unique<Camera2D>();
}

void GameScene::init()
{
    this->prepareScene();
    this->addSystems();

    this->worldDrawer = std::make_unique<WorldDrawer>(WorldDrawer::Config{
        .bus                = this->eventBus,
        .renderer           = this->renderer,
        .camera             = *this->camera,
        .settings           = this->settings,
        .resourceManager    = this->resourceManager,
        .textureLoader      = this->textureLoader
    });
}

void GameScene::update(float deltaTime)
{
    Scene::update(deltaTime);

    this->sweepTimer += deltaTime;
    if (this->sweepTimer >= 5.f)
    {
        this->resourceManager.sweep();
        this->sweepTimer = 0.f;
    }
}

void GameScene::render()
{
    RenderContext ctx{ this->world(), this->eventBus };
    this->worldDrawer->draw(ctx);
}

void GameScene::onExit()
{
    this->inputContext.reset();
    this->camera.reset();
    this->characterLoader.reset();
    this->worldDrawer.reset();
    this->entityFactory.reset();
    this->charDefLoader.reset();
    this->animLoader.reset();
    this->fsmLoader.reset();
    this->stateMachineRegistry.reset();

    this->hitboxLoader.reset();
    this->hurtboxLoader.reset();
    this->pushboxLoader.reset();

    this->mapRoot = Entity{0};
    
    this->world().clear();
    this->resourceManager.sweep();
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

    const auto& mapComp = comp.get<MapComponent>(this->mapRoot);

    float spawnX = 400.f;
    for (const auto& sp : mapComp.spawnPoints) if (sp.playerId == slot.playerId)
    { spawnX = sp.x; break; }
    
    auto& transform = comp.get<TransformComponent>(entity);
    transform.position.x = spawnX;

    if (comp.has<SpriteComponent>(entity))
    {
        const auto& sprite = comp.get<SpriteComponent>(entity);
        transform.position.y = mapComp.floorY - (sprite.size.height * 0.5f);
    }
    else transform.position.y = mapComp.floorY - 32.f;

    if (comp.has<StateComponent>(entity)) comp.get<StateComponent>(entity).current = StateId::Idle;

    LOG_DEBUG("GameScene: prepared player {} entity {} at ({}, {})",
        slot.playerId, entity.id, transform.position.x, transform.position.y);

    LOG_DEBUG("GameScene::preparePlayer: player {} entity {} completed. hasSprite={} hasRender={} pos=({},{})",
        slot.playerId, entity.id,
        comp.has<SpriteComponent>(entity),
        comp.has<RenderComponent>(entity),
        transform.position.x, transform.position.y);
}

void GameScene::addSystems()
{
    auto& systems = this->systemManager;
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

    systems.addSystem<StateSystem>(events, *this->stateMachineRegistry);

    systems.addSystem<HorizontalMovementSystem>(300.f);
    systems.addSystem<JumpSystem>(events, -500.f);
    systems.addSystem<FaceOffSystem>(events);

    auto& mapComp = this->world().components().get<MapComponent>(this->mapRoot);

    systems.addSystem<AirFrictionSystem>(mapComp.airFriction);
    // systems.addSystem<GravitySystem>(mapComp.gravity);
    systems.addSystem<MovementSystem>();
    systems.addSystem<LocalToWorldSystem>();

    systems.addSystem<AnimationStateSystem>(events);
    systems.addSystem<AnimationSystem>();

    auto& collisionDetect = systems.addSystem<CollisionDetectionSystem>(1);
    collisionDetect.addDetector(std::make_unique<RectRectCollisionDetection>());
    collisionDetect.addDetector(std::make_unique<RectCircleCollisionDetection>());
    collisionDetect.addDetector(std::make_unique<CircleCircleCollisionDetection>());

    systems.addSystem<GroundDetectionSystem>(events);
    systems.addSystem<StaticPushboxResolutionSystem>(events);
    systems.addSystem<DynamicPushboxResolutionSystem>(events);
    systems.addSystem<FrictionSystem>(mapComp.floorFriction);

    systems.addSystem<DamageSystem>(events);

    auto& collisionCtrl = systems.addSystem<CollisionControllerSystem>(this->eventBus);
    collisionCtrl.addController(std::make_unique<HitboxCollisionController>());
    collisionCtrl.addController(std::make_unique<HurtboxCollisionController>());
    collisionCtrl.addController(std::make_unique<PushboxCollisionController>());

    systems.addSystem<HitboxControllerSystem>();
    systems.addSystem<HurtboxControllerSystem>();
    systems.addSystem<PushboxControllerSystem>();

    systems.addSystem<CameraControllerSystem>(CameraControllerSystem::Config{
        .camera             = *this->camera,
        .window             = this->window,
        .minZoom            = 0.85f,
        .maxZoom            = 1.05f,
        .padding            = 30.f,
        .verticalOffset     = -70.f,
        .bounds             = mapComp.worldBounds,
        .smoothFactor       = 0.12f,
        .viewSize           = GameConstants::VIRTUAL_SIZE,
        .applyZoomToSize    = true,
    });

    LOG_DEBUG("World bounds: left={}, right={}, top={}, bottom={}",
        mapComp.worldBounds.left, mapComp.worldBounds.right,
        mapComp.worldBounds.top, mapComp.worldBounds.bottom);
}
