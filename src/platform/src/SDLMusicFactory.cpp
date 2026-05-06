#include "../include/SDLMusicFactory/SDLMusicFactory.h"

#include <iostream>
#include <SDL_mixer.h>

std::shared_ptr<Music> SDLMusicFactory::createMusic(const std::string& path)
{
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music)
    {
        std::cerr << "[SDLMusicFactory] Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
        return nullptr;
    }
    return std::make_shared<SDLMusic>(music);
}
