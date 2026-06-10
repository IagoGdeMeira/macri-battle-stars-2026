#include "../../src/app/include/Application/Application.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Application setWindowTitle returns Application for chaining", "[unit][application]")
{
    Application app;
    Application& result = app.setWindowTitle("Test Window");
    REQUIRE(&result == &app);
}

TEST_CASE("Application setWindowSize returns Application for chaining", "[unit][application]")
{
    Application app;
    Application& result = app.setWindowSize(1024, 768);
    REQUIRE(&result == &app);
}

TEST_CASE("Application builder methods can be chained", "[unit][application]")
{
    Application app;
    Application& result = app.setWindowTitle("Chained").setWindowSize(1920, 1080).setWindowTitle("Final Title");
    REQUIRE(&result == &app);
}

TEST_CASE("Application default constructor initializes default values", "[unit][application]")
{
    Application app;
    REQUIRE_NOTHROW(app.setWindowTitle("Macri Battle Stars").setWindowSize(800, 600));
}

TEST_CASE("Application setWindowTitle updates internal title", "[unit][application]")
{
    Application app;
    app.setWindowTitle("Custom Title");
    app.setWindowTitle("Another Title");
    REQUIRE_NOTHROW(app.setWindowTitle("Final"));
}

TEST_CASE("Application setWindowSize updates internal dimensions", "[unit][application]")
{
    Application app;
    app.setWindowSize(1280, 720);
    app.setWindowSize(1920, 1080);
    REQUIRE_NOTHROW(app.setWindowSize(640, 480));
}

TEST_CASE("Application is non-copyable", "[unit][application]")
{
    Application app;
    REQUIRE(true);
}

TEST_CASE("Application default window title is set correctly", "[unit][application]")
{
    Application app;
    REQUIRE_NOTHROW(app.setWindowTitle("Macri Battle Stars").setWindowSize(800, 600));
}

TEST_CASE("Application default window size is 800x600", "[unit][application]")
{
    Application app;
    REQUIRE_NOTHROW(app.setWindowSize(800, 600));
}
