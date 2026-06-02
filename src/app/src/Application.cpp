#include "../include/Application/Application.h"

#include "../engine/include/ResourceManager/ResourceManager.h"
#include "../engine/include/TextureLoader/TextureLoader.h"

#include "../game/scenes/TitleScene.h"

#include "../platform/include/JsonParser/JsonParser.h"
#include "../platform/include/SDLPlatformFactory/SDLPlatformFactory.h"
#include "../platform/include/SDLSystemInitializer/SDLSystemInitializer.h"

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
}

void Application::initLoaders()
{
    this->parser = std::make_unique<JsonParser>();
    this->threadPool = std::make_unique<ThreadPool>(1);
    this->resourceManager = std::make_unique<ResourceManager>(*this->threadPool);
    auto texFactory = this->platformFactory->createTextureFactory(*this->renderer);
    this->textureLoader = std::make_unique<TextureLoader>(*texFactory);
}

void Application::setupInitialScene()
{
    this->engine = std::make_unique<Engine>(*this->window, this->gameSettings);
    this->engine->setRenderer(*this->renderer);

    auto fontFactory = this->platformFactory->createFontFactory();
    auto textureFactory = this->platformFactory->createTextureFactory(*this->renderer);

    this->sceneFactory = std::make_unique<SceneFactory>(SceneFactory::Config{
        .window =           *this->window,
        .parser =           *this->parser,
        .resourceManager =  *this->resourceManager,
        .textureLoader =    *this->textureLoader,
        .renderer =         *this->renderer,
        .eventBus =         this->engine->events(),
        .sceneManager =     this->engine->scenes(),
        .settings =         this->gameSettings,
        .engine =           *this->engine,
        .fontFactory =      *fontFactory,
        .textureFactory =   *textureFactory
    });

    this->engine->setSceneFactory(*this->sceneFactory);

    TitleScene::Config titleCfg;
    this->engine->scenes().changeScene<TitleScene>(std::move(titleCfg));
}
