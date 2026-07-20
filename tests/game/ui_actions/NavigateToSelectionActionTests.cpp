#include "game/ui_actions/NavigateToSelectionAction.h"

#include "StubDataParser.h"
#include "StubEngine.h"
#include "StubFontFactory.h"
#include "StubPlatformFactory.h"
#include "StubRenderer.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"
#include "StubWindow.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/Engine/Engine.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Scene/Scene.h"
#include "engine/include/SceneFactory/SceneFactory.h"
#include "engine/include/SceneManager/SceneManager.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

#include "game/scenes/SelectionScene.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class NavigateToSelectionActionFixture
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

    NavigateToSelectionActionFixture()
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

TEST_CASE_METHOD(NavigateToSelectionActionFixture, "NavigateToSelectionAction changes to Selection scene",
    "[unit][navigate_to_selection_action]"
) {
    NavigateToSelectionAction action(this->scenes());
    action.execute();

    REQUIRE(this->scenes().currentScene() != nullptr);
    REQUIRE(dynamic_cast<SelectionScene*>(this->scenes().currentScene()) != nullptr);
}
