#include "../../src/engine/include/SceneFactory/SceneFactory.h"

#include "../../src/domain/include/Geometry/Geometry.h"
#include "../../src/domain/include/Color/Color.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../src/engine/include/Viewport/Viewport.h"
#include "../../src/engine/include/Window/Window.h"

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <thread>

class SceneFactoryFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& w, int& h) override { w = 800; h = 600; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}
        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}
    };

    class StubDataParser : public DataParser
    {
    public:
        std::unique_ptr<DataNode> parse(const std::string&) const override { return nullptr; }
    };

    class StubResourceManager : public ResourceManager
    {
    public:
        StubResourceManager() : ResourceManager(pool) {}
    private:
        ThreadPool pool{1};
    };

    class StubTextureFactory : public ITextureFactory
    {
    public:
        std::shared_ptr<Texture> createTexture(const std::string&) override { return nullptr; }
    };

    class StubFontFactory : public IFontFactory
    {
    public:
        std::shared_ptr<Font> createFont(const std::string&) override { return nullptr; }
    };

    class StubTextureLoader : public TextureLoader
    {
    public:
        StubTextureLoader() : TextureLoader(texFactory) {}
    private:
        StubTextureFactory texFactory;
    };

    void createEngineAndFactory()
    {
        this->engine = std::make_unique<Engine>(this->window, this->settings);
        this->engine->setRenderer(this->renderer);

        this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
            .window             = this->window,
            .parser             = this->parser,
            .resourceManager    = this->resourceManager,
            .textureLoader      = this->textureLoader,
            .renderer           = this->renderer,
            .eventBus           = this->engine->events(),
            .sceneManager       = this->engine->scenes(),
            .settings           = this->settings,
            .engine             = *this->engine,
            .fontFactory        = this->fontFactory,
            .textureFactory     = this->textureFactory
        });

        this->engine->setSceneFactory(*this->factory);
    }

    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<SceneFactory> factory;
    StubDataParser parser;
    StubResourceManager resourceManager;
    StubTextureLoader textureLoader;
    StubTextureFactory textureFactory;
    StubFontFactory fontFactory;
};

class VerifiableScene : public Scene
{
public:
    struct Config : public Scene::Config {};

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
};

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory creates a scene via template",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scene = this->factory->createScene<VerifiableScene>(std::move(cfg));

    REQUIRE(scene != nullptr);
    REQUIRE(dynamic_cast<VerifiableScene*>(scene.get()) != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory injects all common dependencies",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scenePtr = this->factory->createScene<VerifiableScene>(std::move(cfg));
    auto& scene = static_cast<VerifiableScene&>(*scenePtr);

    REQUIRE(scene.injectedEventBus          == &this->engine->events());
    REQUIRE(scene.injectedSceneManager      == &this->engine->scenes());
    REQUIRE(scene.injectedRenderer          == &this->renderer);
    REQUIRE(scene.injectedWindow            == &this->window);
    REQUIRE(scene.injectedParser            == &this->parser);
    REQUIRE(scene.injectedResourceManager   == &this->resourceManager);
    REQUIRE(scene.injectedTextureLoader     == &this->textureLoader);
    REQUIRE(scene.injectedSettings          == &this->settings);
    REQUIRE(scene.injectedEngine            == this->engine.get());
    REQUIRE(scene.injectedFontFactory       == &this->fontFactory);
    REQUIRE(scene.injectedTextureFactory    == &this->textureFactory);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory throws when renderer is not set in Engine",
    "[unit][scene_factory]"
) {
    this->engine = std::make_unique<Engine>(this->window, this->settings);
    this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
        .window             = this->window,
        .parser             = this->parser,
        .resourceManager    = this->resourceManager,
        .textureLoader      = this->textureLoader,
        .renderer           = this->renderer,
        .eventBus           = this->engine->events(),
        .sceneManager       = this->engine->scenes(),
        .settings           = this->settings,
        .engine             = *this->engine,
        .fontFactory        = this->fontFactory,
        .textureFactory     = this->textureFactory
    });
    this->engine->setSceneFactory(*this->factory);

    VerifiableScene::Config cfg;
    REQUIRE_THROWS(this->factory->createScene<VerifiableScene>(std::move(cfg)));
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory does not require pre-filled config fields",
    "[unit][scene_factory]"
) {
    this->createEngineAndFactory();

    VerifiableScene::Config cfg;
    auto scenePtr = this->factory->createScene<VerifiableScene>(std::move(cfg));
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
