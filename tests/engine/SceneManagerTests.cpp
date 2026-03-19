#include "../../src/engine/include/SceneManager/SceneManager.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager defers scene change until update",
    "[unit][scene_manager]"
) {
    struct TestScene : Scene
    {
        bool updated = false;

        using Scene::Scene;

        void update(float) override { this->updated = true; }
    };

    EventBus bus;
    SceneManager manager;

    manager.changeScene<TestScene>(bus);

    manager.update(0.0f);

    SUCCEED();
}

TEST_CASE("SceneManager calls init before update",
    "[unit][scene_manager]"
) {
    struct TestScene : Scene
    {
        bool initialized = false;
        bool updated = false;

        using Scene::Scene;

        void init() override { this->initialized = true; }

        void update(float) override
        {
            REQUIRE(this->initialized);
            this->updated = true;
        }
    };

    EventBus bus;
    SceneManager manager;

    manager.changeScene<TestScene>(bus);

    manager.update(0.016f);

    SUCCEED();
}

TEST_CASE("SceneManager updates current scene",
    "[unit][scene_manager]"
) {
    struct TestScene : Scene
    {
        int updates = 0;

        using Scene::Scene;

        void update(float) override { this->updates++; }
    };

    EventBus bus;
    SceneManager manager;

    manager.changeScene<TestScene>(bus);

    manager.update(0.016f);
    manager.update(0.016f);
}

TEST_CASE("SceneManager keeps updating scene",
    "[unit][scene_manager]"
) {
    struct TestScene : Scene
    {
        int& updates;

        TestScene(EventBus& bus, int& updates) : Scene(bus), updates(updates) {}

        void update(float) override { this->updates++; }
    };

    EventBus bus;
    SceneManager manager;

    int updates = 0;

    manager.changeScene<TestScene>(bus, updates);

    manager.update(0.016f);
    manager.update(0.016f);

    REQUIRE(updates == 2);
}

TEST_CASE("SceneManager replaces current scene",
    "[unit][scene_manager]"
) {
    struct SceneA : Scene
    {
        int& updatesA;

        SceneA(EventBus& bus, int& updatesA) : Scene(bus), updatesA(updatesA) {}

        void update(float) override { this->updatesA++; }
    };

    struct SceneB : Scene
    {
        int& updatesB;

        SceneB(EventBus& bus, int& updatesB) : Scene(bus), updatesB(updatesB) {}

        void update(float) override { this->updatesB++; }
    };

    EventBus bus;
    SceneManager manager;

    int updatesA = 0;
    int updatesB = 0;

    manager.changeScene<SceneA>(bus, updatesA);
    manager.update(0.016f);

    manager.changeScene<SceneB>(bus, updatesB);
    manager.update(0.016f);

    manager.update(0.016f);

    REQUIRE(updatesA == 1);
    REQUIRE(updatesB == 2);
}

TEST_CASE("SceneManager keeps only last scheduled scene",
    "[unit][scene_manager]"
) {
    struct SceneA : Scene
    {
        int& updates;

        SceneA(EventBus& bus, int& updates) : Scene(bus), updates(updates) {}

        void update(float) override { this->updates++; }
    };

    struct SceneB : Scene
    {
        int& updates;

        SceneB(EventBus& bus, int& updates) : Scene(bus), updates(updates) {}

        void update(float) override { this->updates++; }
    };

    EventBus bus;
    SceneManager manager;

    int updatesA = 0;
    int updatesB = 0;

    manager.changeScene<SceneA>(bus, updatesA);
    manager.changeScene<SceneB>(bus, updatesB);

    manager.update(0.016f);

    REQUIRE(updatesA == 0);
    REQUIRE(updatesB == 1);
}

TEST_CASE("SceneManager update without scene does nothing",
    "[unit][scene_manager]"
) {
    SceneManager manager;

    REQUIRE_NOTHROW(manager.update(0.016f));
}
