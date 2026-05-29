#include "../include/Application/Application.h"

#include "../../engine/include/InputManager/InputManager.h"
#include "../../engine/include/ResourceLoader/SyncLoader.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"

#include "../../game/scenes/TitleScene/TitleScene.h"

#include "../../platform/include/JsonParser/JsonParser.h"
#include "../../platform/include/SDLPlatformFactory/SDLPlatformFactory.h"
#include "../../platform/include/SDLSystemInitializer/SDLSystemInitializer.h"

Application& Application::setWindowTitle(const std::string& title)
{ this->windowTitle = title; return *this; }

Application& Application::setWindowSize(int width, int height)
{ this->windowSize = {static_cast<float>(width), static_cast<float>(height)}; return *this; }

Application& Application::setInitialScene(Scene::Type type)
{ this->initialScene = type; return *this; }

int Application::run()
{
    this->initializer = std::make_unique<SDLSystemInitializer>();
    this->initializer->initialize();

    this->platformFactory = std::make_unique<SDLPlatformFactory>();

    this->initSystems();
    this->initLoaders();

    this->setupInitialScene();
    this->setupInput();

    this->engine->run();
    return 0;
}

void Application::initSystems()
{
    this->window = this->platformFactory->createWindow(this->windowSize.width, this->windowSize.height, this->windowTitle);
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

void Application::setupInput()
{
    // Os adaptadores de entrada serão configurados pela cena inicial, que carrega o InputContext.
    // Por enquanto, o método pode ficar vazio ou ser chamado pela cena.
}

void Application::setupInitialScene()
{
    this->sceneFactory = std::make_unique<SceneFactory>(SceneFactory::Config
    {
        .window = *this->window,
        .parser = *this->parser,
        .resourceManager = *this->resourceManager,
        .textureLoader = *this->textureLoader
    });

    this->engine = std::make_unique<Engine>(*this->window, *this->sceneFactory, this->gameSettings);
    this->sceneFactory->engine = this->engine.get();
    this->engine->setRenderer(*this->renderer);

    TitleScene::Config titleCfg;
    this->engine->scenes().changeScene<TitleScene>(std::move(titleCfg));
}
