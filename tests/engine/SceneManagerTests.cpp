#include "../../src/engine/include/SceneManager/SceneManager.h"

#include "../../src/domain/value_objects/Color/Color.h"
#include "../../src/domain/value_objects/Geometry/Geometry.h"

#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"

#include "../stubs/StubWindow.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubDataParser.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubTextureLoader.h"
#include "../stubs/StubEngine.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class SceneManagerFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<StubEngine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    StubDataParser dataParser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;

    SceneManagerFixture()
    {
        this->engine = std::make_unique<StubEngine>(this->window, this->settings);
        this->engine->setRenderer(this->renderer);

        this->textureFactory = std::make_unique<StubTextureFactory>();
        this->fontFactory = std::make_unique<StubFontFactory>();
        this->textureLoader = std::make_unique<StubTextureLoader>(*this->textureFactory);

        this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
            .window             = this->window,
            .parser             = this->dataParser,
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

    SceneManager& scenes() { return this->engine->scenes(); }
};

class CounterScene : public Scene
{
public:
    struct Config : public Scene::Config { int* counter = nullptr; };

    explicit CounterScene(Config &&cfg) : Scene(*cfg.eventBus), counter(cfg.counter) {}

    void update(float) override { if (this->counter) ++(*this->counter); }

private:
    int* counter;
};

class LifecycleScene : public Scene
{
public:
    struct Config : public Scene::Config
    { int *updates = nullptr, *renders = nullptr, *enters = nullptr, *exits = nullptr, *pauses = nullptr, *resumes = nullptr; };

    explicit LifecycleScene(Config&& cfg) :
        Scene(*cfg.eventBus),
        updates(cfg.updates),
        renders(cfg.renders),
        enters(cfg.enters),
        exits(cfg.exits),
        pauses(cfg.pauses),
        resumes(cfg.resumes) {}

    void update(float) override { if (this->updates) ++(*this->updates); }
    void render() override      { if (this->renders) ++(*this->renders); }
    void onEnter() override     { if (this->enters) ++(*this->enters); }
    void onExit() override      { if (this->exits) ++(*this->exits); }
    void onPause() override     { if (this->pauses) ++(*this->pauses); }
    void onResume() override    { if (this->resumes) ++(*this->resumes); }

private:
    int *updates, *renders, *enters, *exits, *pauses, *resumes;
};

TEST_CASE_METHOD(SceneManagerFixture, "Change scene initializes and updates", "[unit][scene_manager]")
{
    int counter = 0;
    CounterScene::Config cfg;
    cfg.counter = &counter;

    this->scenes().changeScene<CounterScene>(std::move(cfg));
    this->scenes().update(0.016f);
    this->scenes().update(0.016f);

    REQUIRE(counter == 2);
}

TEST_CASE_METHOD(SceneManagerFixture, "Replace scene stops old one", "[unit][scene_manager]")
{
    int firstC = 0, secondC = 0;

    CounterScene::Config cfg1;
    cfg1.counter = &firstC;
    this->scenes().changeScene<CounterScene>(std::move(cfg1));
    this->scenes().update(0.016f);

    CounterScene::Config cfg2;
    cfg2.counter = &secondC;
    this->scenes().changeScene<CounterScene>(std::move(cfg2));
    this->scenes().update(0.016f);
    this->scenes().update(0.016f);

    REQUIRE(firstC == 1);
    REQUIRE(secondC == 2);
}

TEST_CASE_METHOD(SceneManagerFixture, "Lifecycle callbacks are invoked", "[unit][scene_manager]")
{
    int enters = 0, exits = 0, pauses = 0, resumes = 0;

    LifecycleScene::Config cfg1;
    cfg1.enters = &enters;
    cfg1.exits = &exits;
    cfg1.pauses = &pauses;
    cfg1.resumes = &resumes;

    this->scenes().changeScene<LifecycleScene>(std::move(cfg1));
    REQUIRE(enters == 1);

    LifecycleScene::Config cfg2;
    int enters2 = 0, exits2 = 0;
    cfg2.enters = &enters2;
    cfg2.exits = &exits2;

    this->scenes().changeScene<LifecycleScene>(std::move(cfg2));
    REQUIRE(exits == 1);
    REQUIRE(enters2 == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "Push and pop scenes", "[unit][scene_manager]")
{
    int updatesBase = 0, pausesBase = 0, resumesBase = 0;
    int updatesTop = 0, entersTop = 0, exitsTop = 0;

    class AlwaysUpdateScene : public LifecycleScene
    {
    public:
        using LifecycleScene::LifecycleScene;
        UpdatePolicy getUpdatePolicy() const override { return UpdatePolicy::Always; }
    };

    AlwaysUpdateScene::Config baseCfg;
    baseCfg.updates = &updatesBase;
    baseCfg.pauses = &pausesBase;
    baseCfg.resumes = &resumesBase;
    this->scenes().changeScene<AlwaysUpdateScene>(std::move(baseCfg));

    LifecycleScene::Config topCfg;
    topCfg.updates = &updatesTop;
    topCfg.enters = &entersTop;
    topCfg.exits = &exitsTop;
    this->scenes().pushScene(std::make_unique<LifecycleScene>(std::move(topCfg)));

    REQUIRE(pausesBase == 1);
    REQUIRE(entersTop == 1);

    this->scenes().update(0.016f);
    REQUIRE(updatesTop == 1);
    REQUIRE(updatesBase == 1);

    this->scenes().popScene();
    REQUIRE(exitsTop == 1);
    REQUIRE(resumesBase == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "Render calls all scenes", "[unit][scene_manager]")
{
    int renders1 = 0, renders2 = 0;

    LifecycleScene::Config cfg1;
    cfg1.renders = &renders1;
    this->scenes().changeScene<LifecycleScene>(std::move(cfg1));

    LifecycleScene::Config cfg2;
    cfg2.renders = &renders2;
    this->scenes().pushScene(std::make_unique<LifecycleScene>(std::move(cfg2)));

    this->scenes().render();
    REQUIRE(renders1 == 1);
    REQUIRE(renders2 == 1);
}
