#include "SDLSoundFactory/SDLSoundFactory.h"

#include <iostream>
#include <SDL_mixer.h>

std::shared_ptr<Sound> SDLSoundFactory::createSound(const std::string& path)
{
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk)
    {
        std::cerr << "[SDLSoundFactory] Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
        return nullptr;
    }
    return std::make_shared<SDLSound>(chunk);
}
