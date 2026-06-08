#include "../../src/engine/include/Engine/Engine.h"

#include "../../src/domain/include/Color/Color.h"
#include "../../src/domain/include/Geometry/Geometry.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/DrawCommands/DrawCommands.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../src/engine/include/Viewport/Viewport.h"
#include "../../src/engine/include/Window/Window.h"

#include <atomic>
#include <chrono>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <thread>

class EngineFixture
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
        StubTextureLoader(ITextureFactory& factory) : TextureLoader(factory) {}
    };

    void setupEngineWithSceneManager()
    {
        this->engine = std::make_unique<Engine>(this->window, this->settings);
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

    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    StubDataParser parser;
    StubResourceManager resourceManager;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;
    std::unique_ptr<StubTextureLoader> textureLoader;
};

TEST_CASE_METHOD(EngineFixture, "Engine run and stop via QuitEvent", "[unit][engine]")
{
    this->setupEngineWithSceneManager();

    std::atomic<bool> stopped{false};
    this->engine->events().subscribe<QuitEvent>([&](const QuitEvent&) { stopped = true; });

    auto t = std::thread([this]() { this->engine->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    this->engine->events().emit<QuitEvent>();
    t.join();

    REQUIRE(stopped);
}

TEST_CASE_METHOD(EngineFixture, "Engine stops when QuitEvent emitted during loop", "[unit][engine]")
{
    this->setupEngineWithSceneManager();

    std::atomic<bool> stopped{false};
    this->engine->events().subscribe<QuitEvent>([&](const QuitEvent&) { stopped = true; });

    auto t = std::thread([this]() { this->engine->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    this->engine->events().emit<QuitEvent>();
    t.join();

    REQUIRE(stopped);
}
