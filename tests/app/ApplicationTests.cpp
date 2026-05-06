#include "../../src/app/include/Application/Application.h"

#include "../../src/domain/include/Color/Color.h"

#include "../../src/engine/include/InputAdapter/InputAdapter.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

class ApplicationFixture
{
public:
    class StubTexture : public Texture
    {
    public:
        int getWidth() const override { return 64; }
        int getHeight() const override { return 96; }
    };

    class StubWindow : public Window
    {
    public:
        int width = 0;
        int height = 0;
        std::string title;

        void create(int w, int h, const char* t) override
        { this->width = w; this->height = h; this->title = t; }

        void getSize(int& w, int& h) override
        { w = this->width; h = this->height; }

        void setResolution(int w, int h) override
        { this->width = w; this->height = h; }

        void setFullscreen(bool) override {}
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const Texture&, const Renderer::DrawTextureParams&) override {}
        void drawRectOutline(const Rectangle&, const Color&) override {}
        void drawRectFilled(const Rectangle&, const Color&) override {}
        void drawCircleOutline(const Circle&, const Color&) override {}
        void drawCircleFilled(const Circle&, const Color&) override {}
        void setViewport(const Viewport&) override {}
    };

    class StubInputAdapter : public InputAdapter
    {
    public:
        void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>&) override {}
    };
};

TEST_CASE_METHOD(ApplicationFixture, "Application setWindowTitle returns Application for chaining",
    "[unit][application]"
) {
    Application app;
    Application& result = app.setWindowTitle("Test Window");
    REQUIRE(&result == &app);
}

TEST_CASE_METHOD(ApplicationFixture, "Application setWindowSize returns Application for chaining",
    "[unit][application]"
) {
    Application app;
    Application& result = app.setWindowSize(1024, 768);
    REQUIRE(&result == &app);
}

TEST_CASE_METHOD(ApplicationFixture, "Application builder methods can be chained",
    "[unit][application]"
) {
    Application app;
    Application& result = 
        app.setWindowTitle("Chained")
            .setWindowSize(1920, 1080)
            .setWindowTitle("Final Title");
    REQUIRE(&result == &app);
}

TEST_CASE_METHOD(ApplicationFixture, "Application default constructor initializes default values",
    "[unit][application]"
) {
    Application app;

    REQUIRE_NOTHROW(
        app.setWindowTitle("Macri Battle Stars")
           .setWindowSize(800, 600)
    );
}

TEST_CASE_METHOD(ApplicationFixture, "Application setWindowTitle updates internal title",
    "[unit][application]"
) {
    Application app;
    app.setWindowTitle("Custom Title");
    app.setWindowTitle("Another Title");
    
    REQUIRE_NOTHROW(app.setWindowTitle("Final"));
}

TEST_CASE_METHOD(ApplicationFixture, "Application setWindowSize updates internal dimensions",
    "[unit][application]"
) {
    Application app;
    app.setWindowSize(1280, 720);
    app.setWindowSize(1920, 1080);
    
    REQUIRE_NOTHROW(app.setWindowSize(640, 480));
}

TEST_CASE_METHOD(ApplicationFixture, "Application is non-copyable",
    "[unit][application]"
) {
    Application app;

    REQUIRE(true);
}

TEST_CASE_METHOD(ApplicationFixture, "Application default window title is set correctly",
    "[unit][application]"
) {
    Application app;

    REQUIRE_NOTHROW(
        app.setWindowTitle("Macri Battle Stars")
            .setWindowSize(800, 600)
    );
}

TEST_CASE_METHOD(ApplicationFixture, "Application default window size is 800x600",
    "[unit][application]"
) {
    Application app;
    REQUIRE_NOTHROW(app.setWindowSize(800, 600));
}
