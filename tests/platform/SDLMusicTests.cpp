#include "../../src/platform/include/SDLMusic/SDLMusic.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <SDL_mixer.h>

class SDLMusicFixture
{
public:
    void configureAudioDriverForCi() const
    {
        #if defined(__linux__)
                SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        #endif
    }

    bool initAudio() const
    {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) return false;
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) { SDL_Quit(); return false; }
        return true;
    }

    void shutdownAudio() const
    {
        Mix_CloseAudio();
        Mix_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_Quit();
    }
};

TEST_CASE_METHOD(SDLMusicFixture, "SDLMusic handles nullptr gracefully",
    "[unit][sdl_music]"
) {
    SDLMusic music(nullptr);

    REQUIRE(music.getVolume() == 100);

    music.play();
    music.stop();
    music.pause();
    music.resume();

    REQUIRE(music.getVolume() == 100);
}

TEST_CASE_METHOD(SDLMusicFixture, "SDLMusic setVolume after audio init",
    "[integration][sdl_music]"
) {
    this->configureAudioDriverForCi();

    if (!this->initAudio()) return;

    SDLMusic music(nullptr);
    music.setVolume(30);
    REQUIRE(music.getVolume() == 30);

    this->shutdownAudio();
}
