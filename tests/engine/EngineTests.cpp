#include "../../src/engine/include/Engine/Engine.h"

#include "../stubs/StubDataParser.h"
#include "../stubs/StubEngine.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"
#include "../stubs/StubWindow.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <thread>

class EngineFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    StubDataParser parser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubEngine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;
    std::unique_ptr<StubTextureLoader> textureLoader;

    void setupEngineWithSceneManager()
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
