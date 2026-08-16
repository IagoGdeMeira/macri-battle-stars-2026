#include "Application/Application.h"

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/events/WindowResizedEvent.h"

#include "game/scenes/GameScene.h"
#include "game/scenes/HUDScene.h"

#include "platform/include/JsonParser/JsonParser.h"
#include "platform/include/SDLPlatformFactory/SDLPlatformFactory.h"
#include "platform/include/SDLSystemInitializer/SDLSystemInitializer.h"

Application& Application::setWindowTitle(const std::string& title)
{
    this->windowTitle = title;
    return *this;
}

Application& Application::setWindowSize(int width, int height)
{
    this->windowSize = {static_cast<float>(width), static_cast<float>(height)};
    return *this;
}

int Application::run()
{
    this->initializer = std::make_unique<SDLSystemInitializer>();
    this->initializer->initialize();
    this->platformFactory = std::make_unique<SDLPlatformFactory>();

    this->initSystems();
    this->initLoaders();
    this->setupInitialScene();

    this->engine->run();
    return 0;
}

void Application::initSystems()
{
    int sizeW = static_cast<int>(this->windowSize.width), sizeH = static_cast<int>(this->windowSize.height);
    this->window = this->platformFactory->createWindow(sizeW, sizeH, this->windowTitle);
    this->renderer = this->platformFactory->createRenderer(*this->window);

    int w, h;
    this->window->getSize(w, h);
    this->gameSettings.screen.size = {static_cast<float>(w), static_cast<float>(h)};
}

void Application::initLoaders()
{
    this->parser = std::make_unique<JsonParser>();
    this->threadPool = std::make_unique<ThreadPool>(1);
    this->resourceManager = std::make_unique<ResourceManager>(*this->threadPool);

    this->textureFactory = this->platformFactory->createTextureFactory(*this->renderer);
    this->textureLoader = std::make_unique<TextureLoader>(*this->textureFactory);
}

void Application::setupInitialScene()
{
    this->fontFactory = this->platformFactory->createFontFactory();
    this->engine = std::make_unique<Engine>(*this->window, this->gameSettings);
    this->engine->setRenderer(*this->renderer);

    auto& events = this->engine->events();
    events.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e)
    {
        this->gameSettings.screen.size = e.newSize;
        LOG_DEBUG("Window resized to {}x{}, updating screen size", e.newSize.width, e.newSize.height);
    });

    this->sceneFactory = std::make_unique<SceneFactory>(SceneFactory::Config{
        .window             = *this->window,
        .parser             = *this->parser,
        .resourceManager    = *this->resourceManager,
        .textureLoader      = *this->textureLoader,
        .renderer           = *this->renderer,
        .eventBus           = events,
        .settings           = this->gameSettings,
        .engine             = *this->engine,
        .fontFactory        = *this->fontFactory,
        .textureFactory     = *this->textureFactory,
        .platformFactory    = this->platformFactory.get()
    });

    this->sceneManager = std::make_unique<SceneManager>(*this->sceneFactory, *this->engine);
    this->engine->setSceneManager(*this->sceneManager);
    auto& scenes = this->engine->scenes();

    GameScene::Config gameCfg;
    gameCfg.playerSlots = {
        {0, GameConstants::DEFAULT_CHARACTER_DEF_PATH},
        {1, "assets/characters/grey_beta.json"}
    };
    gameCfg.mapPath             = GameConstants::DEFAULT_MAP_PATH;
    gameCfg.inputBindingsPath   = GameConstants::DEFAULT_INPUT_BINDINGS_PATH;
    gameCfg.combosPath          = GameConstants::DEFAULT_COMBOS_PATH;
    gameCfg.triggersPath        = GameConstants::DEFAULT_TRIGGERS_PATH;
    gameCfg.roundTime           = 99.f;
    scenes.changeScene<GameScene>(std::move(gameCfg));

    HUDScene::Config hudCfg;
    hudCfg.initialRoundTime = 99.f;
    hudCfg.layoutPath = "assets/ui/hud_layout.json";
    hudCfg.healthBarWidgetPath = "assets/ui/widgets/health_bar.json";
    auto hudScene = this->sceneFactory->createScene<HUDScene>(std::move(hudCfg), this->sceneManager.get());
    scenes.pushScene(std::move(hudScene));
}
