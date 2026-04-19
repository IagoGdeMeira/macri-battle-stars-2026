#include "../../src/engine/include/SceneManager/SceneManager.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneId/SceneId.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <variant>

class SceneManagerFixture
{
public:
    struct CounterData { int* updates; };

    struct LifecycleData
    {
        int *updates, *renders, *enters, *exits, *pauses, *resumes;
        Scene::UpdatePolicy policy = Scene::UpdatePolicy::WhenTop;
    };

    struct CounterTracker
    {
        int updates = 0;

        CounterData asData() { return CounterData{ &this->updates }; }
    };

    struct LifecycleTracker
    {
        int updates = 0;
        int renders = 0;
        int enters = 0;
        int exits = 0;
        int pauses = 0;
        int resumes = 0;

        LifecycleData asData(Scene::UpdatePolicy policy = Scene::UpdatePolicy::WhenTop)
        {
            return LifecycleData
            {
                &this->updates, &this->renders, &this->enters, &this->exits,
                &this->pauses, &this->resumes, policy
            };
        }
    };

    class FlagScene : public Scene
    {
    public:
        using Scene::Scene;

        void init() override { this->initialized = true; }
        void update(float) override { this->updated = true; }

        bool initialized = false;
        bool updated = false;
    };

    class CounterScene : public Scene
    {
    public:
        CounterScene(EventBus& bus, CounterData data) :
            Scene(bus), data(data) {}

        void update(float) override { ++(*this->data.updates); }

    private:
        CounterData data;
    };

    class LifecycleScene : public Scene
    {
    public:
        LifecycleScene(EventBus& bus, LifecycleData data) :
            Scene(bus), data(data) {}

        void update(float) override { ++(*this->data.updates); }
        void render() override { ++(*this->data.renders); }
        void onEnter() override { ++(*this->data.enters); }
        void onExit() override { ++(*this->data.exits); }
        void onPause() override { ++(*this->data.pauses); }
        void onResume() override { ++(*this->data.resumes); }
        UpdatePolicy getUpdatePolicy() const override { return this->data.policy; }

    private:
        LifecycleData data;
    };

    class TestSceneFactory : public ISceneFactory
    {
    public:
        std::unique_ptr<Scene> createScene(SceneId id, std::any data) override
        {
            if (id == SceneId::Title && data.type() == typeid(std::monostate))
            { return std::make_unique<FlagScene>(this->bus); }

            if (data.type() == typeid(CounterData))
            {
                auto cfg = std::any_cast<CounterData>(std::move(data));
                return std::make_unique<CounterScene>(this->bus, cfg);
            }

            if (data.type() == typeid(LifecycleData))
            {
                auto cfg = std::any_cast<LifecycleData>(std::move(data));
                return std::make_unique<LifecycleScene>(this->bus, cfg);
            }

            throw std::runtime_error("Unsupported SceneId/data combination in test factory");
        }

    private:
        EventBus bus;
    };

    void changeCounter(SceneId id, CounterTracker& tracker)
    { this->manager.changeScene(id, tracker.asData()); }

    void pushCounter(SceneId id, CounterTracker& tracker)
    { this->manager.pushScene(id, tracker.asData()); }

    void changeLifecycle(
        SceneId id,
        LifecycleTracker& tracker,
        Scene::UpdatePolicy policy = Scene::UpdatePolicy::WhenTop
    ) { this->manager.changeScene(id, tracker.asData(policy)); }

    void pushLifecycle(
        SceneId id,
        LifecycleTracker& tracker,
        Scene::UpdatePolicy policy = Scene::UpdatePolicy::WhenTop
    ) { this->manager.pushScene(id, tracker.asData(policy)); }

    FlagScene* currentFlagScene()
    { return dynamic_cast<FlagScene*>(this->manager.currentScene()); }

    TestSceneFactory factory;
    SceneManager manager { this->factory };
};

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager defers scene change until update",
    "[unit][scene_manager]"
) {
    this->manager.changeScene(SceneId::Title, std::monostate{});

    this->manager.update(0.0f);

    auto* scene = this->currentFlagScene();
    REQUIRE(scene != nullptr);
    REQUIRE(scene->updated);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager calls init before update",
    "[unit][scene_manager]"
) {
    this->manager.changeScene(SceneId::Title, std::monostate{});

    auto* scene = this->currentFlagScene();
    REQUIRE(scene != nullptr);
    REQUIRE(scene->initialized);

    this->manager.update(0.016f);
    REQUIRE(scene->updated);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager updates current scene",
    "[unit][scene_manager]"
) {
    CounterTracker tracker;

    this->changeCounter(SceneId::Selection, tracker);

    this->manager.update(0.016f);
    this->manager.update(0.016f);

    REQUIRE(tracker.updates == 2);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager keeps updating scene",
    "[unit][scene_manager]"
) {
    CounterTracker tracker;

    this->changeCounter(SceneId::Selection, tracker);

    this->manager.update(0.016f);
    this->manager.update(0.016f);

    REQUIRE(tracker.updates == 2);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager replaces current scene",
    "[unit][scene_manager]"
) {
    CounterTracker first;
    CounterTracker second;

    this->changeCounter(SceneId::Selection, first);
    this->manager.update(0.016f);

    this->changeCounter(SceneId::Game, second);
    this->manager.update(0.016f);

    this->manager.update(0.016f);

    REQUIRE(first.updates == 1);
    REQUIRE(second.updates == 2);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager keeps only last scheduled scene",
    "[unit][scene_manager]"
) {
    CounterTracker first;
    CounterTracker second;

    this->changeCounter(SceneId::Selection, first);
    this->changeCounter(SceneId::Game, second);

    this->manager.update(0.016f);

    REQUIRE(first.updates == 0);
    REQUIRE(second.updates == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager update without scene does nothing",
    "[unit][scene_manager]"
) {
    REQUIRE_NOTHROW(this->manager.update(0.016f));
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager changeScene exits old scene and enters new scene",
    "[unit][scene_manager]"
) {
    LifecycleTracker first;
    LifecycleTracker second;

    this->changeLifecycle(SceneId::Selection, first);
    this->changeLifecycle(SceneId::Game, second);

    REQUIRE(first.enters == 1);
    REQUIRE(first.exits == 1);
    REQUIRE(first.pauses == 0);
    REQUIRE(first.resumes == 0);
    REQUIRE(second.enters == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager pushScene pauses previous and popScene resumes it",
    "[unit][scene_manager]"
) {
    LifecycleTracker first;
    LifecycleTracker second;

    this->changeLifecycle(SceneId::Selection, first, Scene::UpdatePolicy::Always);
    this->pushLifecycle(SceneId::Game, second, Scene::UpdatePolicy::WhenTop);

    REQUIRE(first.pauses == 1);
    REQUIRE(second.enters == 1);

    this->manager.popScene();

    REQUIRE(second.exits == 1);
    REQUIRE(first.resumes == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager update policy controls which stacked scenes update",
    "[unit][scene_manager]"
) {
    LifecycleTracker alwaysTracker;
    LifecycleTracker neverTracker;

    this->changeLifecycle(SceneId::Selection, alwaysTracker, Scene::UpdatePolicy::Always);
    this->pushLifecycle(SceneId::Game, neverTracker, Scene::UpdatePolicy::Never);

    this->manager.update(0.016f);
    this->manager.update(0.016f);

    REQUIRE(alwaysTracker.updates == 2);
    REQUIRE(neverTracker.updates == 0);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager render calls render on all stacked scenes",
    "[unit][scene_manager]"
) {
    LifecycleTracker first;
    LifecycleTracker second;

    this->changeLifecycle(SceneId::Selection, first, Scene::UpdatePolicy::Always);
    this->pushLifecycle(SceneId::Game, second, Scene::UpdatePolicy::WhenTop);

    this->manager.render();

    REQUIRE(first.renders == 1);
    REQUIRE(second.renders == 1);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager empty/currentScene reflect stack state",
    "[unit][scene_manager]"
) {
    LifecycleTracker tracker;

    REQUIRE(this->manager.empty());
    REQUIRE(this->manager.currentScene() == nullptr);

    this->changeLifecycle(SceneId::Selection, tracker);

    REQUIRE_FALSE(this->manager.empty());
    REQUIRE(this->manager.currentScene() != nullptr);

    this->manager.popScene();

    REQUIRE(this->manager.empty());
    REQUIRE(this->manager.currentScene() == nullptr);
}

TEST_CASE_METHOD(SceneManagerFixture, "SceneManager popScene on empty stack is no-op",
    "[unit][scene_manager]"
) { REQUIRE_NOTHROW(this->manager.popScene()); }

TEST_CASE("SceneManager throws when factory returns nullptr",
    "[unit][scene_manager]"
) {
    class NullFactory : public ISceneFactory
    {
    public:
        std::unique_ptr<Scene> createScene(SceneId, std::any) override { return nullptr; }
    };

    NullFactory factory;
    SceneManager manager(factory);

    REQUIRE_THROWS(manager.changeScene(SceneId::Title, std::monostate{}));
}
