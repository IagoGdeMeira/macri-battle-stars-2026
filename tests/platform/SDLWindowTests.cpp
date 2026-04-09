#include "../../src/platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

class SDLWindowFixture
{
public:
    void configureVideoDriverForCi() const
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
        #endif
    }
};

TEST_CASE_METHOD(SDLWindowFixture, "SDLWindow starts with close flag disabled",
    "[integration][sdl_window]"
) {
    SDLWindow window;
    REQUIRE(window.getNativeHandle() == nullptr);

    SDL_Quit();
}

TEST_CASE_METHOD(SDLWindowFixture, "SDLWindow creates native window and reports configured size",
    "[integration][sdl_window]"
) {
    this->configureVideoDriverForCi();

    {
        SDLWindow window;
        window.create(800, 600, "Window Test");

        REQUIRE(window.getNativeHandle() != nullptr);

        int width = 0;
        int height = 0;
        window.getSize(width, height);

        REQUIRE(width == 800);
        REQUIRE(height == 600);
    }

    SDL_Quit();
}

TEST_CASE_METHOD(SDLWindowFixture, "SDLWindow updates size after setResolution",
    "[integration][sdl_window]"
) {
    this->configureVideoDriverForCi();

    {
        SDLWindow window;
        window.create(800, 600, "Window Test");

        window.setResolution(1024, 768);

        int width = 0;
        int height = 0;
        window.getSize(width, height);

        REQUIRE(width == 1024);
        REQUIRE(height == 768);

        REQUIRE_NOTHROW(window.setFullscreen(true));
        REQUIRE_NOTHROW(window.setFullscreen(false));
    }

    SDL_Quit();
}
