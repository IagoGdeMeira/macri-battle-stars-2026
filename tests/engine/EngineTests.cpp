#include "../src/engine/include/Engine/Engine.h"

#include "../src/domain/include/Color/Color.h"

#include "../src/engine/events/PlatformEvent.h"
#include "../src/engine/events/QuitEvent.h"
#include "../src/engine/include/GameSettings/GameSettings.h"
#include "../src/engine/include/InputAdapter/InputAdapter.h"
#include "../src/engine/include/IPlatformEventProvider/IPlatformEventProvider.h"
#include "../src/engine/include/Scene/Scene.h"
#include "../src/engine/include/SceneId/SceneId.h"
#include "../src/engine/include/Window/Window.h"

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

    EngineFixture() : engine(window, factory, settings) { this->factory.setEventBus(this->engine.events()); }

protected:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& width, int& height) override { width = 0; height = 0; }
    };

    StubWindow window;
    TestSceneFactory factory;
    GameSettings settings;
    Engine engine;
};

TEST_CASE_METHOD(EngineFixture, "Engine must stop when QuitEvent is emitted",
    "[unit][engine]"
) {
    this->engine.scenes().changeScene(SceneId::Title, std::monostate{});

    REQUIRE_NOTHROW(this->engine.run());
}

TEST_CASE_METHOD(EngineFixture, "Engine must keep looping until stopped",
    "[unit][engine]"
) {
    int updates = 0;
    const int maxUpdates = 5;

    this->engine.scenes().changeScene(SceneId::Selection, CountedQuitSceneData{ &updates, maxUpdates });
    this->engine.run();

    REQUIRE(updates == maxUpdates);
}

TEST_CASE_METHOD(EngineFixture, "Engine stops when stop is called", 
    "[unit][engine]"
) {
    this->engine.scenes().changeScene(SceneId::Game, std::monostate{});

    std::thread t([&]() { this->engine.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    this->engine.stop();

    t.join();

    SUCCEED();
}

TEST_CASE_METHOD(EngineFixture, "Engine must poll configured input adapter",
    "[unit][engine]"
) {
    class StubProvider : public IPlatformEventProvider
    {
    public:
        explicit StubProvider(int& polls) : polls(&polls) {}

        std::vector<std::unique_ptr<PlatformEvent>> pollEvents() override
        {
            ++(*this->polls);

            std::vector<std::unique_ptr<PlatformEvent>> events;
            events.push_back(std::make_unique<QuitPlatformEvent>());
            return events;
        }

        int* polls;
    };

    class StubInputAdapter : public InputAdapter
    {
    public:
        StubInputAdapter(EventBus& bus, int& polls) : bus(bus), polls(polls) {}

        void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override
        {
            ++this->polls;
            if (!events.empty()) this->bus.emit<QuitEvent>();
        }

    private:
        EventBus& bus;
        int& polls;
    };

    int providerPolls = 0;
    int adapterPolls = 0;

    this->engine.input().setProvider(std::make_unique<StubProvider>(providerPolls));
    this->engine.input().addAdapter(std::make_unique<StubInputAdapter>(this->engine.events(), adapterPolls));
    this->engine.scenes().changeScene(SceneId::Game, std::monostate{});
    this->engine.run();

    REQUIRE(providerPolls > 0);
    REQUIRE(adapterPolls > 0);
}

TEST_CASE_METHOD(EngineFixture, "Engine presents frames when renderer is configured",
    "[unit][engine]"
) {
    class StubTexture : public Texture
    {
    public:
        int getWidth() const override { return 0; }
        int getHeight() const override { return 0; }
    };

    class StubRenderer : public Renderer
    {
    public:
        int presentCalls = 0;

        void clear() override {}
        void present() override { ++this->presentCalls; }
        
        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}

        void setViewport(const Viewport&) override {}
    };

    StubRenderer renderer;
    this->engine.setRenderer(renderer);
    this->engine.scenes().changeScene(SceneId::Title, std::monostate{});

    this->engine.run();

    REQUIRE(renderer.presentCalls == 1);
}

TEST_CASE_METHOD(EngineFixture, "Engine render phase calls scene render",
    "[unit][engine]"
) {
    int renders = 0;

    this->engine.scenes().changeScene(SceneId::Game, RenderQuitSceneData{ &renders, 3 });
    this->engine.run();

    REQUIRE(renders == 3);
}

TEST_CASE_METHOD(EngineFixture, "Engine run works without renderer configured",
    "[unit][engine]"
) {
    this->engine.scenes().changeScene(SceneId::Title, std::monostate{});

    REQUIRE_NOTHROW(this->engine.run());
}
