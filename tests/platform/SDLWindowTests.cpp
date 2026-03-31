#include "../../src/platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

TEST_CASE("SDLWindow starts with close flag disabled",
    "[integration][sdl_window]"
) {
    SDLWindow window;
    REQUIRE(window.shouldClose() == false);
}

TEST_CASE("SDLWindow marks close requested when receiving SDL_QUIT",
    "[integration][sdl_window]"
) {
    SDLWindow window;

    SDL_Event quitEvent {};
    quitEvent.type = SDL_QUIT;

    REQUIRE(SDL_PushEvent(&quitEvent) == 1);

    window.pollEvents();

    REQUIRE(window.shouldClose() == true);
}
