#include "../include/SDLMusic/SDLMusic.h"

#include <algorithm>

SDLMusic::~SDLMusic() { if (this->music) Mix_FreeMusic(this->music); }

void SDLMusic::play(bool loop)
{
    if (!this->music) return;
    Mix_VolumeMusic(scaleVolume(this->volume));
    Mix_PlayMusic(this->music, loop ? -1 : 0);
}

void SDLMusic::stop() { Mix_HaltMusic(); }
void SDLMusic::pause() { Mix_PauseMusic(); }
void SDLMusic::resume() { Mix_ResumeMusic(); }

void SDLMusic::setVolume(int newVolume)
{
    this->volume = std::clamp(newVolume, 0, 100);
    Mix_VolumeMusic(scaleVolume(this->volume));
}

int SDLMusic::getVolume() const { return this->volume; }

bool SDLMusic::isPlaying() const { return Mix_PlayingMusic() != 0; }

int SDLMusic::scaleVolume(int userVolume)
{
    userVolume = std::clamp(userVolume, 0, 100);
    return static_cast<int>(userVolume * SDLMusic::VOLUME_SCALE);
}
