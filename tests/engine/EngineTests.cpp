#include "../../src/engine/include/Engine/Engine.h"

#include "../stubs/StubDataParser.h"
#include "../stubs/StubEngine.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"
#include "../stubs/StubWindow.h"
#include "../stubs/StubSceneManager.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"

#include <atomic>
#include <catch2/catch_approx.hpp>
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
    std::unique_ptr<StubSceneManager> sceneManager;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<SceneFactory> factory;

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

        this->sceneManager = std::make_unique<StubSceneManager>(*this->factory, *this->engine);
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

TEST_CASE_METHOD(EngineFixture, "Engine uses targetFPS from GameSettings", "[unit][engine]")
{
    this->settings.targetFPS = 30;
    this->setupEngineWithSceneManager();

    auto t = std::thread([this]() { this->engine->run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    this->engine->stop();
    t.join();

    int expectedUpdates = 3;
    int actualUpdates = this->sceneManager->updateCalls;
    REQUIRE(actualUpdates >= expectedUpdates - 1);
    REQUIRE(actualUpdates <= expectedUpdates + 2);

    for (float delta : this->sceneManager->deltas) REQUIRE(delta == Catch::Approx(1.f / 30.f).margin(0.005f));
}

TEST_CASE_METHOD(EngineFixture, "Engine handles different FPS configurations without crashing", "[unit][engine]")
{
    std::vector<int> fpsValues = { 30, 60, 120, 144 };

    for (int fps : fpsValues)
    {
        this->settings.targetFPS = fps;
        this->setupEngineWithSceneManager();

        auto t = std::thread([this]() { this->engine->run(); });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->engine->stop();
        t.join();

        REQUIRE(this->sceneManager->updateCalls > 0);
    }
}

TEST_CASE_METHOD(EngineFixture, "Engine uses default FPS from GameConstants when not overridden", "[unit][engine]")
{
    this->setupEngineWithSceneManager();

    auto t = std::thread([this]() { this->engine->run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    this->engine->stop();
    t.join();

    REQUIRE(this->sceneManager->updateCalls > 0);
    for (float delta : this->sceneManager->deltas) REQUIRE(delta == Catch::Approx(1.f / 60.f).margin(0.005f));    
}
