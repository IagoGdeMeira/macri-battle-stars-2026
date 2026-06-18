#include "../../src/game/include/UIActionFactory/UIActionFactory.h"

#include "../stubs/StubDataParser.h"
#include "../stubs/StubEngine.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubPlatformFactory.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"
#include "../stubs/StubWindow.h"

#include "../../src/domain/value_objects/Color/Color.h"
#include "../../src/domain/value_objects/Geometry/Geometry.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"

#include "../../src/game/ui_actions/NavigateToSelectionAction.h"
#include "../../src/game/ui_actions/NavigateToTitleAction.h"
#include "../../src/game/ui_actions/QuitAction.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>

class UIActionFactoryFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<StubEngine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<UIActionFactory> actionFactory;
    StubDataParser dataParser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;

    UIActionFactoryFixture()
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

        UIActionFactory::Config cfg{ this->engine->events(), this->engine->scenes() };
        this->actionFactory = std::make_unique<UIActionFactory>(cfg);
    }
};

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates NavigateToSelectionAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory->create("NavigateToSelection");
    REQUIRE(dynamic_cast<NavigateToSelectionAction*>(action.get()) != nullptr);
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates NavigateToTitleAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory->create("NavigateToTitle");
    REQUIRE(dynamic_cast<NavigateToTitleAction*>(action.get()) != nullptr);
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates QuitAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory->create("Quit");
    REQUIRE(dynamic_cast<QuitAction*>(action.get()) != nullptr);
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory rejects unknown action types",
    "[unit][ui_action_factory]"
) { REQUIRE_THROWS_AS(this->actionFactory->create("Unknown"), std::runtime_error); }
