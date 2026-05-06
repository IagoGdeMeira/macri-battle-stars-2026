#ifndef sdl_sound_h
#define sdl_sound_h

#include "../../../engine/include/Sound/Sound.h"

#include <SDL_mixer.h>

class SDLSound : public Sound
{
public:
    SDLSound(Mix_Chunk* chunk) : chunk(chunk) {}
    ~SDLSound() override;

    void play(bool loop = false) override;
    void stop() override;
    void setVolume(int newVolume) override;
    int  getVolume() const override;
    bool isPlaying() const override;

private:
    static constexpr float VOLUME_SCALE = 128.0f / 100.0f;

    Mix_Chunk* chunk;
    int channel = -1;
    int volume = 100;

    static int scaleVolume(int userVolume);
};

#endif // sdl_sound_h
