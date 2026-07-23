#include "platform/draw_commands/SDLDrawCircleCommandHandler.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <SDL.h>

class SDLDrawCircleCommandHandlerFixture
{
public:
    SDLDrawCircleCommandHandlerFixture()
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif

        REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

        this->window.create(800, 600, "Circle Handler Test");
        this->renderer = SDL_CreateRenderer(this->window.get(), -1, SDL_RENDERER_SOFTWARE);
        REQUIRE(this->renderer != nullptr);
    }

    ~SDLDrawCircleCommandHandlerFixture()
    {
        if (this->renderer) SDL_DestroyRenderer(this->renderer);
        SDL_Quit();
    }

    SDL_Renderer* getRenderer() const { return this->renderer; }

private:
    SDLWindow window;
    SDL_Renderer* renderer = nullptr;
};

TEST_CASE_METHOD(SDLDrawCircleCommandHandlerFixture, "SDLDrawCircleCommandHandler draws outlined circle",
    "[unit][sdl_draw_circle_command_handler]"
) {
    SDLDrawCircleCommandHandler handler(this->getRenderer());

    DrawCircleCommand cmd;
    cmd.circle = Circle{Position{100.f, 100.f}, 30.f};
    cmd.color = Color{0, 0, 255, 255};
    cmd.filled = false;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawCircleCommandHandlerFixture, "SDLDrawCircleCommandHandler draws filled circle",
    "[unit][sdl_draw_circle_command_handler]"
) {
    SDLDrawCircleCommandHandler handler(this->getRenderer());

    DrawCircleCommand cmd;
    cmd.circle = Circle{Position{150.f, 100.f}, 25.f};
    cmd.color = Color{255, 0, 255, 200};
    cmd.filled = true;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawCircleCommandHandlerFixture, "SDLDrawCircleCommandHandler handles zero radius",
    "[unit][sdl_draw_circle_command_handler]"
) {
    SDLDrawCircleCommandHandler handler(this->getRenderer());

    DrawCircleCommand cmd;
    cmd.circle = Circle{Position{50.f, 50.f}, 0.f};
    cmd.color = Color::WHITE();
    cmd.filled = true;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}
