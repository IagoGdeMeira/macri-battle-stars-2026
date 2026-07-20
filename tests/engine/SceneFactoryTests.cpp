#include "engine/include/SceneFactory/SceneFactory.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/events/QuitEvent.h"
#include "engine/include/Engine/Engine.h"
#include "engine/include/Scene/Scene.h"
#include "engine/include/SceneManager/SceneManager.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

#include "StubDataParser.h"
#include "StubEngine.h"
#include "StubFontFactory.h"
#include "StubRenderer.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"
#include "StubWindow.h"

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <thread>

class SceneFactoryFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<StubEngine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    StubDataParser parser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;

    void createEngineAndFactory()
    {
        this->engine = std::make_unique<StubEngine>(this->window, this->settings);
        this->engine->setRenderer(this->renderer);

        this->textureFactory = std::make_unique<StubTextureFactory>();
        this->fontFactory = std::make_unique<StubFontFactory>();
        this->textureLoader = std::make_unique<StubTextureLoader>(*this->textureFactory);

        this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
            .window             = this->window,
            .parser             = this->parser,
            .resourceManager    = this->resourceManager,
            .textureLoader      = *this->textureLoader,
            .renderer           = this->renderer,
            .eventBus           = this->engine->events(),
            .settings           = this->settings,
            .engine             = *this->engine,
            .fontFactory        = *this->fontFactory,
            .textureFactory     = *this->textureFactory
        });

        this->sceneManager = std::make_unique<SceneManager>(*this->factory, *this->engine);
        this->engine->setSceneManager(*this->sceneManager);
    }
};

class VerifiableScene : public Scene
{
public:
    struct Config : public Scene::Config {};

    EventBus* injectedEventBus = nullptr;
    SceneManager* injectedSceneManager = nullptr;
    Renderer* injectedRenderer = nullptr;
    Window* injectedWindow = nullptr;
    DataParser* injectedParser = nullptr;
    ResourceManager* injectedResourceManager = nullptr;
    TextureLoader* injectedTextureLoader = nullptr;
    GameSettings* injectedSettings = nullptr;
    Engine* injectedEngine = nullptr;
    IFontFactory* injectedFontFactory = nullptr;
    ITextureFactory* injectedTextureFactory = nullptr;

    explicit VerifiableScene(Config&& cfg) : Scene(*cfg.eventBus)
    {
        this->injectedEventBus          = cfg.eventBus;
        this->injectedSceneManager      = cfg.sceneManager;
        this->injectedRenderer          = cfg.renderer;
        this->injectedWindow            = cfg.window;
        this->injectedParser            = cfg.parser;
        this->injectedResourceManager   = cfg.resourceManager;
        this->injectedTextureLoader     = cfg.textureLoader;
        this->injectedSettings          = cfg.settings;
        this->injectedEngine            = cfg.engine;
        this->injectedFontFactory       = cfg.fontFactory;
        this->injectedTextureFactory    = cfg.textureFactory;
    }
};

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory creates a scene via template",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scene = this->factory->createScene<VerifiableScene>(std::move(cfg), this->sceneManager.get());

    REQUIRE(scene != nullptr);
    REQUIRE(dynamic_cast<VerifiableScene*>(scene.get()) != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory injects all common dependencies",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scenePtr = this->factory->createScene<VerifiableScene>(std::move(cfg), this->sceneManager.get());
    auto& scene = static_cast<VerifiableScene&>(*scenePtr);

    REQUIRE(scene.injectedEventBus          == &this->engine->events());
    REQUIRE(scene.injectedSceneManager      == this->sceneManager.get());
    REQUIRE(scene.injectedRenderer          == &this->renderer);
    REQUIRE(scene.injectedWindow            == &this->window);
    REQUIRE(scene.injectedParser            == &this->parser);
    REQUIRE(scene.injectedResourceManager   == &this->resourceManager);
    REQUIRE(scene.injectedTextureLoader     == this->textureLoader.get());
    REQUIRE(scene.injectedSettings          == &this->settings);
    REQUIRE(scene.injectedEngine            == this->engine.get());
    REQUIRE(scene.injectedFontFactory       == this->fontFactory.get());
    REQUIRE(scene.injectedTextureFactory    == this->textureFactory.get());
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory does not require pre-filled config fields",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scenePtr = this->factory->createScene<VerifiableScene>(std::move(cfg), this->sceneManager.get());
    auto& scene = static_cast<VerifiableScene&>(*scenePtr);

    REQUIRE(scene.injectedEventBus != nullptr);
    REQUIRE(scene.injectedSceneManager != nullptr);
    REQUIRE(scene.injectedRenderer != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "Integration: Engine runs a scene created by SceneFactory",
    "[integration][scene_factory]"
) {
    this->createEngineAndFactory();

    class SimpleScene : public Scene
    {
    public:
        struct Config : public Scene::Config {};
        explicit SimpleScene(Config&& cfg) : Scene(*cfg.eventBus) {}
        void update(float) override {}
    };

    SimpleScene::Config sceneCfg;
    this->engine->scenes().changeScene<SimpleScene>(std::move(sceneCfg));

    std::atomic<bool> quit{false};
    this->engine->events().subscribe<QuitEvent>([&](const QuitEvent&) { quit = true; });

    auto t = std::thread([this]() { this->engine->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    this->engine->events().emit<QuitEvent>();
    t.join();

    REQUIRE(quit);
}
