#include "../../src/engine/include/Engine/Engine.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

class Stub
{
public:
    Stub() : engine(window) {}

protected:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const std::string&) override {}
        void pollEvents() override {}
        bool shouldClose() const override { return false; }
    };

    StubWindow window;
    Engine engine;
};

TEST_CASE_METHOD(Stub, "Engine must stop when QuitEvent is emitted",
    "[unit][engine]"
) {
    struct TestScene : Scene
    {
        using Scene::Scene;

        void update(float) override { this->eventBus.emit<QuitEvent>(); }
    };

    engine.scenes().changeScene<TestScene>(engine.events());

    REQUIRE_NOTHROW(engine.run());
}

TEST_CASE_METHOD(Stub, "Engine must keep looping until stopped",
    "[unit][engine]"
) {
    int updates = 0;
    const int maxUpdates = 5;

    struct TestScene : Scene
    {
        int& updates;
        const int& maxUpdates;

        TestScene(EventBus& bus, int& updates, const int& maxUpdates):
            Scene(bus), updates(updates), maxUpdates(maxUpdates) {}

        void update(float) override
        {
            this->updates++;
            if (this->updates >= this->maxUpdates) this->eventBus.emit<QuitEvent>();
        }
    };

    engine.scenes().changeScene<TestScene>(engine.events(), updates, maxUpdates);
    engine.run();

    REQUIRE(updates == maxUpdates);
}

TEST_CASE_METHOD(Stub, "Engine stops when stop is called", 
    "[unit][engine]"
) {
    struct TestScene : Scene
    {
        using Scene::Scene;
        void update(float) override {}
    };

    engine.scenes().changeScene<TestScene>(engine.events());

    std::thread t([&]() { engine.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    engine.stop();

    t.join();

    SUCCEED();
}
