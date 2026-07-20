#include "platform/include/SDLSound/SDLSound.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <SDL_mixer.h>

class SDLSoundFixture
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

TEST_CASE_METHOD(SDLSoundFixture, "SDLSound handles nullptr gracefully", "[unit][sdl_sound]")
{
    SDLSound sound(nullptr);

    REQUIRE(sound.getVolume() == 100);

    sound.play();
    sound.stop();

    sound.setVolume(50);
    REQUIRE(sound.getVolume() == 50);

    REQUIRE(sound.isPlaying() == false);
}

TEST_CASE_METHOD(SDLSoundFixture, "SDLSound setVolume after audio init", "[integration][sdl_sound]")
{
    this->configureAudioDriverForCi();

    if (!this->initAudio()) return;

    SDLSound sound(nullptr);
    sound.setVolume(10);
    REQUIRE(sound.getVolume() == 10);

    this->shutdownAudio();
}
