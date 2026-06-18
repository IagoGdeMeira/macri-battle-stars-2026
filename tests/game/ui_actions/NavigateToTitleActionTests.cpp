#include "../../../src/game/ui_actions/NavigateToTitleAction.h"

#include "../../stubs/StubDataParser.h"
#include "../../stubs/StubEngine.h"
#include "../../stubs/StubFontFactory.h"
#include "../../stubs/StubPlatformFactory.h"
#include "../../stubs/StubRenderer.h"
#include "../../stubs/StubResourceManager.h"
#include "../../stubs/StubTextureFactory.h"
#include "../../stubs/StubTextureLoader.h"
#include "../../stubs/StubWindow.h"

#include "../../../src/domain/value_objects/Color/Color.h"
#include "../../../src/domain/value_objects/Geometry/Geometry.h"

#include "../../../src/engine/include/Engine/Engine.h"
#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/GameSettings/GameSettings.h"
#include "../../../src/engine/include/Scene/Scene.h"
#include "../../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../../src/engine/include/SceneManager/SceneManager.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class DummyScene : public Scene
{
public:
    struct Config : public Scene::Config {};

    explicit DummyScene(Config &&cfg) : Scene(*cfg.eventBus) {}

    void init() override {}
    void update(float) override {}
    void render() override {}
};

class NavigateToDummyAction : public IUIAction
{
public:
    explicit NavigateToDummyAction(SceneManager &sceneManager) : sceneManager(sceneManager) {}
    void execute() const override
    {
        DummyScene::Config cfg;
        this->sceneManager.changeScene<DummyScene>(std::move(cfg));
    }

private:
    SceneManager& sceneManager;
};

class NavigateToTitleActionFixture
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

    NavigateToTitleActionFixture()
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

TEST_CASE_METHOD(NavigateToTitleActionFixture, "NavigateToTitleAction changes to Title scene",
    "[unit][navigate_to_title_action]"
) {
    NavigateToDummyAction action(this->scenes());
    action.execute();

    REQUIRE(this->scenes().currentScene() != nullptr);
    REQUIRE(dynamic_cast<DummyScene *>(this->scenes().currentScene()) != nullptr);
}
