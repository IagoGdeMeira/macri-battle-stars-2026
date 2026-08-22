#include "HUDScene.h"

#include "AlignItemsHandler.h"
#include "BoxModelHandler.h"
#include "ComponentRegistry/HUDComponentRegistry.h"
#include "GrowHandler.h"
#include "HealthBarSystem/HealthBarSystem.h"
#include "HealthBarWidgetLoader.h"
#include "HUDVisibilitySystem/HUDVisibilitySystem.h"
#include "JustifyContentHandler.h"
#include "LocalToWorldSystem/LocalToWorldSystem.h"
#include "MainAxisHandler.h"
#include "PlayerSpawnedEvent.h"
#include "RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"
#include "TimerWidgetLoader.h"
#include "UIActionFactory/UIActionFactory.h"
#include "UIFactory/UIFactory.h"
#include "UIDrawer/UIDrawer.h"
#include "UIFlexLayoutSystem/UIFlexLayoutSystem.h"
#include "UILoader/UILoader.h"

#include "domain/components/FlexContainer.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include "engine/value_objects/GameConstants/GameConstants.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

HUDScene::HUDScene(Config&& cfg) :
    Scene(*cfg.eventBus),
    initialRoundTime(cfg.initialRoundTime),
    layoutPath(cfg.layoutPath),
    healthBarWidgetPath(cfg.healthBarWidgetPath),
    eventBus(*cfg.eventBus),
    sceneManager(*cfg.sceneManager),
    parser(*cfg.parser),
    renderer(*cfg.renderer),
    settings(*cfg.settings),
    fontFactory(*cfg.fontFactory),
    textureFactory(*cfg.textureFactory),
    hudRoot(Entity{0}) {}

HUDScene::~HUDScene() = default;

void HUDScene::init()
{
    this->registerComponents();
    this->addSystems();

    this->uiFactory = std::make_unique<UIFactory>(this->world(), this->fontFactory, this->textureFactory);
    this->uiDrawer = std::make_unique<UIDrawer>(this->eventBus, this->renderer, this->settings);

    this->actionFactory = std::make_unique<UIActionFactory>(UIActionFactory::Config{
        .eventBus = this->eventBus, .sceneManager = this->sceneManager });

    this->uiLoader = std::make_unique<UILoader>(this->parser, *this->uiFactory, *this->actionFactory, this->fontFactory);

    this->registerWidgetLoaders();
    this->loadHUDLayout();
    this->prepareHealthBars();
}

void HUDScene::onPause() { if (this->visibilitySystem) this->visibilitySystem->setVisible(false); }

void HUDScene::onResume() { if (this->visibilitySystem) this->visibilitySystem->setVisible(true); }

void HUDScene::render()
{
    if (!this->uiDrawer) return;
    RenderContext ctx{ this->world(), this->eventBus };
    this->uiDrawer->draw(ctx);
}

void HUDScene::registerComponents() { HUDComponentRegistry::registerAll(this->world().components()); }

void HUDScene::addSystems()
{
    auto& flexSystem = this->addSystem<UIFlexLayoutSystem>();
    flexSystem.addHandler(std::make_unique<BoxModelHandler>());
    flexSystem.addHandler(std::make_unique<GrowHandler>());
    flexSystem.addHandler(std::make_unique<JustifyContentHandler>());
    flexSystem.addHandler(std::make_unique<MainAxisHandler>());
    flexSystem.addHandler(std::make_unique<AlignItemsHandler>());

    this->addSystem<LocalToWorldSystem>();
    this->addSystem<HealthBarSystem>(this->eventBus);
    this->addSystem<RoundTimerDisplaySystem>(this->eventBus);

    auto& visSystem = this->addSystem<HUDVisibilitySystem>();
    this->visibilitySystem = &visSystem;
}

void HUDScene::loadHUDLayout()
{
    auto entities = this->uiLoader->loadLayout(this->layoutPath);
    if (!entities.empty()) this->hudRoot = entities[0];
}

void HUDScene::registerWidgetLoaders()
{
    this->uiLoader->registerWidgetLoader("healthBar", std::make_unique<HealthBarWidgetLoader>(*this->uiFactory));
    this->uiLoader->registerWidgetLoader("timer", std::make_unique<TimerWidgetLoader>(*this->uiFactory, this->fontFactory));
}

void HUDScene::prepareHealthBars()
{
    auto leftContainerOpt  = this->uiLoader->findEntityById("leftHealthBarContainer");
    auto rightContainerOpt = this->uiLoader->findEntityById("rightHealthBarContainer");
    if (!leftContainerOpt.has_value() || !rightContainerOpt.has_value()) return;

    Entity leftContainer  = *leftContainerOpt;
    Entity rightContainer = *rightContainerOpt;

    this->eventBus.subscribe<PlayerSpawnedEvent>([this, leftContainer, rightContainer](const PlayerSpawnedEvent& e)
    {
        UILoader::ParamMap params;
        params["playerId"]      = std::to_string(e.playerId);
        params["maxHealth"]     = std::to_string(e.maxHealth);
        params["currentHealth"] = std::to_string(e.currentHealth);

        Entity target = (e.playerId == 0) ? leftContainer : rightContainer;
        this->uiLoader->instantiateWidget(this->healthBarWidgetPath, params, target);
    });
}
