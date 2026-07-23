#include "platform/draw_commands/SDLDrawRectangleCommandHandler.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <SDL.h>

class SDLDrawRectangleCommandHandlerFixture
{
public:
    SDLDrawRectangleCommandHandlerFixture()
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif

        REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

        this->window.create(800, 600, "Rectangle Handler Test");
        this->renderer = SDL_CreateRenderer(this->window.get(), -1, SDL_RENDERER_SOFTWARE);
        REQUIRE(this->renderer != nullptr);
    }

    ~SDLDrawRectangleCommandHandlerFixture()
    {
        if (this->renderer) SDL_DestroyRenderer(this->renderer);
        SDL_Quit();
    }

    SDL_Renderer* getRenderer() const { return this->renderer; }

private:
    SDLWindow window;
    SDL_Renderer* renderer = nullptr;
};

TEST_CASE_METHOD(SDLDrawRectangleCommandHandlerFixture, "SDLDrawRectangleCommandHandler draws outlined rectangle",
    "[unit][sdl_draw_rectangle_command_handler]"
) {
    SDLDrawRectangleCommandHandler handler(this->getRenderer());

    DrawRectangleCommand cmd;
    cmd.rect = Rectangle{Position{10.f, 10.f}, Dimension2D{50.f, 30.f}};
    cmd.color = Color{255, 0, 0, 255};
    cmd.filled = false;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawRectangleCommandHandlerFixture, "SDLDrawRectangleCommandHandler draws filled rectangle",
    "[unit][sdl_draw_rectangle_command_handler]"
) {
    SDLDrawRectangleCommandHandler handler(this->getRenderer());

    DrawRectangleCommand cmd;
    cmd.rect = Rectangle{Position{20.f, 20.f}, Dimension2D{50.f, 30.f}};
    cmd.color = Color{0, 255, 0, 128};
    cmd.filled = true;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}
