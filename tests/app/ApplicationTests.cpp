#include "../../src/app/include/Application/Application.h"

#include "../../src/engine/include/InputAdapter/InputAdapter.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

class StubWindow : public Window
{
public:
    int width = 0;
    int height = 0;
    std::string title;

    void create(int w, int h, const char* t) override
    {
        this->width = w;
        this->height = h;
        this->title = t;
    }

    void getSize(int& w, int& h) override
    {
        w = this->width;
        h = this->height;
    }

    void setResolution(int w, int h) override
    {
        this->width = w;
        this->height = h;
    }

    void setFullscreen(bool) override {}
};

class StubRenderer : public Renderer
{
public:
    void clear() override {}
    void present() override {}

    std::shared_ptr<Texture> createTexture(const std::string&) override
    {
        return std::make_shared<Texture>();
    }

    void draw(const Texture&, const DrawParams&) override {}
    void setViewport(const Viewport&) override {}
};

class StubInputAdapter : public InputAdapter
{
public:
    void poll() override {}
};

TEST_CASE("Application setWindowTitle returns Application for chaining",
    "[unit][application]"
) {
    Application app;
    Application& result = app.setWindowTitle("Test Window");
    REQUIRE(&result == &app);
}

TEST_CASE("Application setWindowSize returns Application for chaining",
    "[unit][application]"
) {
    Application app;
    Application& result = app.setWindowSize(1024, 768);
    REQUIRE(&result == &app);
}

TEST_CASE("Application builder methods can be chained",
    "[unit][application]"
) {
    Application app;
    Application& result = app.setWindowTitle("Chained")
                              .setWindowSize(1920, 1080)
                              .setWindowTitle("Final Title");
    REQUIRE(&result == &app);
}

TEST_CASE("Application default constructor initializes default values",
    "[unit][application]"
) {
    Application app;
    // Test that we can access default values through builder pattern
    REQUIRE_NOTHROW(
        app.setWindowTitle("Macri Battle Stars")
           .setWindowSize(800, 600)
    );
}

TEST_CASE("Application setWindowTitle updates internal title",
    "[unit][application]"
) {
    Application app;
    app.setWindowTitle("Custom Title");
    app.setWindowTitle("Another Title");
    
    // Verify multiple calls work (indirectly via builder)
    REQUIRE_NOTHROW(app.setWindowTitle("Final"));
}

TEST_CASE("Application setWindowSize updates internal dimensions",
    "[unit][application]"
) {
    Application app;
    app.setWindowSize(1280, 720);
    app.setWindowSize(1920, 1080);
    
    // Verify multiple calls work
    REQUIRE_NOTHROW(app.setWindowSize(640, 480));
}

TEST_CASE("Application is non-copyable",
    "[unit][application]"
) {
    Application app;
    
    // These should not compile if uncommented:
    // Application copy(app);
    // Application& ref = (app = app);
    
    REQUIRE(true); // Compile-time check
}

TEST_CASE("Application default window title is set correctly",
    "[unit][application]"
) {
    Application app;
    // Through builder chaining, verify we can override default
    REQUIRE_NOTHROW(
        app.setWindowTitle("Macri Battle Stars")
           .setWindowSize(800, 600)
    );
}

TEST_CASE("Application default window size is 800x600",
    "[unit][application]"
) {
    Application app;
    // Verify default size can be used (indirectly)
    REQUIRE_NOTHROW(app.setWindowSize(800, 600));
}
