#include "platform/draw_commands/SDLDrawFontCommandHandler.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "platform/include/SDLWindow/SDLWindow.h"
#include "platform/include/SDLFont/SDLFont.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>

class SDLDrawFontCommandHandlerFixture
{
public:
    SDLDrawFontCommandHandlerFixture()
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif

        REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);
        REQUIRE(TTF_Init() == 0);

        this->window.create(800, 600, "Font Handler Test");
        this->renderer = SDL_CreateRenderer(this->window.get(), -1, SDL_RENDERER_SOFTWARE);
        REQUIRE(this->renderer != nullptr);
    }

    ~SDLDrawFontCommandHandlerFixture()
    {
        if (this->renderer) SDL_DestroyRenderer(this->renderer);
        TTF_Quit();
        SDL_Quit();
    }

    SDL_Renderer* getRenderer() const { return this->renderer; }

private:
    SDLWindow window;
    SDL_Renderer* renderer = nullptr;
};

TEST_CASE_METHOD(SDLDrawFontCommandHandlerFixture, "SDLDrawFontCommandHandler handles null font gracefully",
    "[unit][sdl_draw_font_command_handler]"
) {
    SDLDrawFontCommandHandler handler(this->getRenderer());

    DrawFontCommand cmd;
    cmd.font = nullptr;
    cmd.text = "Hello";
    cmd.dest = Rectangle{Position{10.f, 10.f}, Dimension2D{100.f, 20.f}};
    cmd.fontSize = 12;
    cmd.color = Color::WHITE();

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawFontCommandHandlerFixture, "SDLDrawFontCommandHandler draws with valid font",
    "[unit][sdl_draw_font_command_handler]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) {
        SKIP("Font file not available");
    }

    SDLFont font(fontPath);
    SDLDrawFontCommandHandler handler(this->getRenderer());

    DrawFontCommand cmd;
    cmd.font = &font;
    cmd.text = "Test";
    cmd.dest = Rectangle{Position{50.f, 50.f}, Dimension2D{100.f, 30.f}};
    cmd.fontSize = 18;
    cmd.color = Color{255, 255, 0, 255};

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawFontCommandHandlerFixture, "SDLDrawFontCommandHandler handles empty text",
    "[unit][sdl_draw_font_command_handler]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) {
        SKIP("Font file not available");
    }

    SDLFont font(fontPath);
    SDLDrawFontCommandHandler handler(this->getRenderer());

    DrawFontCommand cmd;
    cmd.font = &font;
    cmd.text = "";
    cmd.dest = Rectangle{Position{50.f, 50.f}, Dimension2D{100.f, 30.f}};
    cmd.fontSize = 18;
    cmd.color = Color::WHITE();

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    SUCCEED();
}
