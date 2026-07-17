#ifndef sdl_music_h
#define sdl_music_h

#include "../../engine/resources/Music/Music.h"

#include <SDL_mixer.h>

class SDLMusic : public Music
{
public:
    explicit SDLMusic(Mix_Music* music) : music(music) {}
    ~SDLMusic() override;

    void play(bool loop = false) override;
    void stop() override;
    void pause() override;
    void resume() override;
    void setVolume(int newVolume) override;
    int  getVolume() const override;
    bool isPlaying() const override;

private:
    static constexpr float VOLUME_SCALE = 128.f / 100.f;

    Mix_Music* music = nullptr;
    int volume = 100;

    static int scaleVolume(int userVolume);
};

#endif // sdl_music_h
