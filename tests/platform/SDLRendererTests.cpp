#include "../../src/platform/include/SDLRenderer/SDLRenderer.h"

#include "../../src/platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

TEST_CASE("SDLRenderer can be created from an SDL window",
    "[integration][sdl_renderer]"
) {
    SDLWindow window;
    window.create(800, 600, "Renderer Test");

    SDLRenderer renderer(window.getNativeHandle());

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.present());
    REQUIRE_NOTHROW(renderer.setViewport(0, 0, 800, 600));

    SDL_Quit();
}
