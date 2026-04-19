#include "../../src/engine/include/Engine/Engine.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/InputAdapter/InputAdapter.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneId/SceneId.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>

class EngineFixture
{
public:
    struct CountedQuitSceneData { int* updates; int maxUpdates; };
    struct RenderQuitSceneData { int* renders; int maxRenders; };

    class QuitOnUpdateScene : public Scene
    {
    public:
        using Scene::Scene;

        void update(float) override { this->eventBus.emit<QuitEvent>(); }
    };

    class CountedQuitScene : public Scene
    {
    public:
        CountedQuitScene(EventBus& bus, CountedQuitSceneData data) :
            Scene(bus), data(data) {}

        void update(float) override
        {
            ++(*this->data.updates);
            if (*this->data.updates >= this->data.maxUpdates) this->eventBus.emit<QuitEvent>();
        }

    private:
        CountedQuitSceneData data;
    };

    class IdleScene : public Scene
    {
    public:
        using Scene::Scene;
        void update(float) override {}
    };

    class RenderCountQuitScene : public Scene
    {
    public:
        RenderCountQuitScene(EventBus& bus, RenderQuitSceneData data) :
            Scene(bus), data(data) {}

        void render() override
        {
            ++(*this->data.renders);
            if (*this->data.renders >= this->data.maxRenders) this->eventBus.emit<QuitEvent>();
        }

    private:
        RenderQuitSceneData data;
    };

    class TestSceneFactory : public ISceneFactory
    {
    public:
        void setEventBus(EventBus& bus) { this->eventBus = &bus; }

        std::unique_ptr<Scene> createScene(SceneId id, std::any data) override
        {
            if (this->eventBus == nullptr) throw std::runtime_error("EventBus not bound");

            switch (id)
            {
                case SceneId::Title:
                    return std::make_unique<QuitOnUpdateScene>(*this->eventBus);
                case SceneId::Selection:
                {
                    auto cfg = std::any_cast<CountedQuitSceneData>(std::move(data));
                    return std::make_unique<CountedQuitScene>(*this->eventBus, cfg);
                }
                case SceneId::Game:
                    if (data.type() == typeid(RenderQuitSceneData))
                    {
                        auto cfg = std::any_cast<RenderQuitSceneData>(std::move(data));
                        return std::make_unique<RenderCountQuitScene>(*this->eventBus, cfg);
                    }
                    return std::make_unique<IdleScene>(*this->eventBus);
                default:
                    throw std::runtime_error("Unsupported SceneId in test factory");
            }
        }

    private:
        EventBus* eventBus = nullptr;
    };

    EngineFixture() : engine(window, factory) { this->factory.setEventBus(this->engine.events()); }

protected:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& width, int& height) override
        {
            width = 0;
            height = 0;
        }
    };

    StubWindow window;
    TestSceneFactory factory;
    Engine engine;
};

TEST_CASE_METHOD(EngineFixture, "Engine must stop when QuitEvent is emitted",
    "[unit][engine]"
) {
    engine.scenes().changeScene(SceneId::Title, std::monostate{});

    REQUIRE_NOTHROW(engine.run());
}

TEST_CASE_METHOD(EngineFixture, "Engine must keep looping until stopped",
    "[unit][engine]"
) {
    int updates = 0;
    const int maxUpdates = 5;

    engine.scenes().changeScene(SceneId::Selection, CountedQuitSceneData{ &updates, maxUpdates });
    engine.run();

    REQUIRE(updates == maxUpdates);
}

TEST_CASE_METHOD(EngineFixture, "Engine stops when stop is called", 
    "[unit][engine]"
) {
    engine.scenes().changeScene(SceneId::Game, std::monostate{});

    std::thread t([&]() { engine.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    engine.stop();

    t.join();

    SUCCEED();
}

TEST_CASE_METHOD(EngineFixture, "Engine must poll configured input adapter",
    "[unit][engine]"
) {
    class StubInputAdapter : public InputAdapter
    {
    public:
        StubInputAdapter(EventBus& bus, int& polls) : bus(bus), polls(polls) {}

        void poll() override
        {
            polls++;
            this->bus.emit<QuitEvent>();
        }

    private:
        EventBus& bus;
        int& polls;
    };

    int polls = 0;
    StubInputAdapter adapter(engine.events(), polls);

    engine.setInputAdapter(adapter);
    engine.scenes().changeScene(SceneId::Game, std::monostate{});
    engine.run();

    REQUIRE(polls > 0);
}

TEST_CASE_METHOD(EngineFixture, "Engine presents frames when renderer is configured",
    "[unit][engine]"
) {
    class StubRenderer : public Renderer
    {
    public:
        int presentCalls = 0;

        void clear() override {}
        void present() override { ++this->presentCalls; }
        std::shared_ptr<Texture> createTexture(const std::string&) override
        {
            return std::make_shared<Texture>();
        }
        void draw(const Texture&, const DrawParams&) override {}
        void setViewport(const Viewport&) override {}
    };

    StubRenderer renderer;
    engine.setRenderer(renderer);
    engine.scenes().changeScene(SceneId::Title, std::monostate{});

    engine.run();

    REQUIRE(renderer.presentCalls == 1);
}

TEST_CASE_METHOD(EngineFixture, "Engine render phase calls scene render",
    "[unit][engine]"
) {
    int renders = 0;

    engine.scenes().changeScene(SceneId::Game, RenderQuitSceneData{ &renders, 3 });
    engine.run();

    REQUIRE(renders == 3);
}

TEST_CASE_METHOD(EngineFixture, "Engine run works without renderer configured",
    "[unit][engine]"
) {
    engine.scenes().changeScene(SceneId::Title, std::monostate{});

    REQUIRE_NOTHROW(engine.run());
}
