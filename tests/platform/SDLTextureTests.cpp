#include "../../src/platform/include/SDLTexture/SDLTexture.h"

#include "../../src/platform/include/SDLRenderer/SDLRenderer.h"
#include "../../src/platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

class SDLTextureFixture
{
public:
    void configureVideoDriverForCi() const
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif
    }
};

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture handles nullptr gracefully",
    "[unit][sdl_texture]"
) {
    SDLTexture texture(nullptr);

    REQUIRE(texture.get() == nullptr);
    REQUIRE(texture.getWidth() == 0);
    REQUIRE(texture.getHeight() == 0);
}

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture stores and retrieves SDL_Texture pointer",
    "[integration][sdl_texture]"
) {
    this->configureVideoDriverForCi();

    SDLWindow window;
    window.create(800, 600, "Texture Test");

    SDL_Renderer* renderer = SDL_CreateRenderer(window.getNativeHandle(), -1, 0);
    REQUIRE(renderer != nullptr);

    SDL_Surface* surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    REQUIRE(sdlTexture != nullptr);

    {
        SDLTexture texture(sdlTexture);
        REQUIRE(texture.get() == sdlTexture);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture reports correct width from SDL_Texture",
    "[integration][sdl_texture]"
) {
    this->configureVideoDriverForCi();

    SDLWindow window;
    window.create(800, 600, "Texture Test");

    SDL_Renderer* renderer = SDL_CreateRenderer(window.getNativeHandle(), -1, 0);
    REQUIRE(renderer != nullptr);

    const int TEXTURE_WIDTH = 256;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, TEXTURE_WIDTH, 128, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    REQUIRE(sdlTexture != nullptr);

    {
        SDLTexture texture(sdlTexture);
        REQUIRE(texture.getWidth() == TEXTURE_WIDTH);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture reports correct height from SDL_Texture",
    "[integration][sdl_texture]"
) {
    this->configureVideoDriverForCi();

    SDLWindow window;
    window.create(800, 600, "Texture Test");

    SDL_Renderer* renderer = SDL_CreateRenderer(window.getNativeHandle(), -1, 0);
    REQUIRE(renderer != nullptr);

    const int TEXTURE_HEIGHT = 192;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 128, TEXTURE_HEIGHT, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    REQUIRE(sdlTexture != nullptr);

    {
        SDLTexture texture(sdlTexture);
        REQUIRE(texture.getHeight() == TEXTURE_HEIGHT);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture destructor releases SDL_Texture",
    "[integration][sdl_texture]"
) {
    this->configureVideoDriverForCi();

    SDLWindow window;
    window.create(800, 600, "Texture Test");

    SDL_Renderer* renderer = SDL_CreateRenderer(window.getNativeHandle(), -1, 0);
    REQUIRE(renderer != nullptr);

    SDL_Surface* surface = SDL_CreateRGBSurface(0, 64, 64, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    REQUIRE(sdlTexture != nullptr);

    {
        SDLTexture texture(sdlTexture);
        REQUIRE(texture.get() != nullptr);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

TEST_CASE_METHOD(SDLTextureFixture, "SDLTexture with both width and height dimensions",
    "[integration][sdl_texture]"
) {
    this->configureVideoDriverForCi();

    SDLWindow window;
    window.create(800, 600, "Texture Test");

    SDL_Renderer* renderer = SDL_CreateRenderer(window.getNativeHandle(), -1, 0);
    REQUIRE(renderer != nullptr);

    const int TEST_WIDTH = 512;
    const int TEST_HEIGHT = 384;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, TEST_WIDTH, TEST_HEIGHT, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
    REQUIRE(sdlTexture != nullptr);

    {
        SDLTexture texture(sdlTexture);
        REQUIRE(texture.getWidth() == TEST_WIDTH);
        REQUIRE(texture.getHeight() == TEST_HEIGHT);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
