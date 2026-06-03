#include "../../src/engine/include/Engine/Engine.h"

#include "../../src/domain/include/Geometry/Geometry.h"
#include "../../src/domain/include/Color/Color.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/DrawCommands/DrawCommands.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/Window/Window.h"
#include "../../src/engine/include/Viewport/Viewport.h"

#include <atomic>
#include <chrono>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <thread>

class EngineFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& w, int& h) override { w = 800; h = 600; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}
        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}
    };

    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
};

TEST_CASE_METHOD(EngineFixture, "Engine run and stop via QuitEvent", "[engine]")
{
    Engine engine(this->window, this->settings);
    engine.setRenderer(this->renderer);

    std::atomic<bool> stopped{false};
    engine.events().subscribe<QuitEvent>([&](const QuitEvent&) { stopped = true; });

    auto t = std::thread([&]() { engine.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    engine.events().emit<QuitEvent>();
    t.join();

    REQUIRE(stopped);
}

TEST_CASE_METHOD(EngineFixture, "Engine stops when QuitEvent emitted during loop", "[engine]")
{
    Engine engine(this->window, this->settings);
    engine.setRenderer(this->renderer);

    std::atomic<bool> stopped{false};
    engine.events().subscribe<QuitEvent>([&](const QuitEvent&) { stopped = true; });

    auto t = std::thread([&]() { engine.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    engine.events().emit<QuitEvent>();
    t.join();

    REQUIRE(stopped);
}
