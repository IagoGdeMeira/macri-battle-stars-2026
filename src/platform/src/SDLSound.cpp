#include "../include/SDLSound/SDLSound.h"

#include <algorithm>
#include <iostream>

SDLSound::~SDLSound() { if (this->chunk) Mix_FreeChunk(this->chunk); }

void SDLSound::play(bool loop)
{
    if (!this->chunk)
    {
        std::cerr << "[SDLSound] Error: No sound chunk to play." << std::endl;
        return;
    }

    Mix_VolumeChunk(this->chunk, SDLSound::scaleVolume(this->volume));

    int loops = loop ? -1 : 0;
    this->channel = Mix_PlayChannel(-1, this->chunk, loops);

    if (this->channel == -1)
    { std::cerr << "[SDLSound] Error: Failed to play sound. Error: " << Mix_GetError() << std::endl; }
}

void SDLSound::stop()
{
    if (this->channel != -1)
    {
        Mix_HaltChannel(this->channel);
        this->channel = -1;
    }
}

void SDLSound::setVolume(int newVolume)
{
    this->volume = std::clamp(newVolume, 0, 100);
    if (this->chunk) Mix_VolumeChunk(this->chunk, SDLSound::scaleVolume(this->volume));
}

int SDLSound::getVolume() const { return this->volume; }

bool SDLSound::isPlaying() const
{ return this->channel != -1 && Mix_Playing(this->channel) != 0; }

int SDLSound::scaleVolume(int userVolume)
{
    userVolume = std::clamp(userVolume, 0, 100);
    return static_cast<int>(userVolume * SDLSound::VOLUME_SCALE);
}
