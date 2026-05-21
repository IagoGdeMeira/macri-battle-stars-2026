#include "../../src/game/include/UIActionFactory/UIActionFactory.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/ISceneFactory/ISceneFactory.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneId/SceneId.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"

#include "../../src/game/ui_actions/NavigateToSelectionAction.h"
#include "../../src/game/ui_actions/NavigateToTitleAction.h"
#include "../../src/game/ui_actions/QuitAction.h"

#include <any>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <stdexcept>
#include <typeinfo>

class UIActionFactoryFixture
{
public:
    class TestSceneFactory : public ISceneFactory
    {
    public:
        std::unique_ptr<Scene> createScene(SceneId id, std::any data) override
        {
            this->lastId = id;
            this->lastData = std::move(data);
            ++this->createSceneCalls;

            return std::make_unique<Scene>(this->eventBus);
        }

        EventBus eventBus;
        SceneId lastId { };
        std::any lastData;
        int createSceneCalls = 0;
    };

    TestSceneFactory factory;
    SceneManager sceneManager { this->factory };
    EventBus eventBus;
    UIActionFactory actionFactory { UIActionFactory::Config { this->eventBus, this->sceneManager } };
};

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates NavigateToSelectionAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory.create("NavigateToSelection");

    REQUIRE(dynamic_cast<NavigateToSelectionAction*>(action.get()) != nullptr);

    action->execute();
    this->sceneManager.update(0.016f);

    REQUIRE(this->factory.lastId == SceneId::Selection);
    REQUIRE(this->factory.lastData.type() == typeid(std::nullptr_t));
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates NavigateToTitleAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory.create("NavigateToTitle");

    REQUIRE(dynamic_cast<NavigateToTitleAction*>(action.get()) != nullptr);

    action->execute();
    this->sceneManager.update(0.016f);

    REQUIRE(this->factory.lastId == SceneId::Title);
    REQUIRE(this->factory.lastData.type() == typeid(std::nullptr_t));
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory creates QuitAction",
    "[unit][ui_action_factory]"
) {
    auto action = this->actionFactory.create("Quit");

    REQUIRE(dynamic_cast<QuitAction*>(action.get()) != nullptr);

    int quitCount = 0;
    this->eventBus.subscribe<QuitEvent>([&](const QuitEvent&) { ++quitCount; });

    action->execute();

    REQUIRE(quitCount == 1);
}

TEST_CASE_METHOD(UIActionFactoryFixture, "UIActionFactory rejects unknown action types",
    "[unit][ui_action_factory]"
) { REQUIRE_THROWS_AS(this->actionFactory.create("Unknown"), std::runtime_error); }
