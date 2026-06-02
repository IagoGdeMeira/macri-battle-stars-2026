#include "../../src/game/ui_actions/NavigateToTitleAction.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/ISceneFactory/ISceneFactory.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/SceneId/SceneId.h"

#include <any>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <typeinfo>

class NavigateToTitleActionFixture
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
    SceneManager manager { this->factory };
};

TEST_CASE_METHOD(NavigateToTitleActionFixture, "NavigateToTitleAction changes to Title scene",
    "[unit][navigate_to_title_action]"
) {
    NavigateToTitleAction action(this->manager);

    action.execute();

    REQUIRE(this->factory.createSceneCalls == 1);
    REQUIRE(this->factory.lastId == SceneId::Title);
    REQUIRE(this->factory.lastData.type() == typeid(std::nullptr_t));
    REQUIRE(this->manager.currentScene() != nullptr);
}
