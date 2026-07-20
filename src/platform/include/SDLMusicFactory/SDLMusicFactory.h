#ifndef sdl_music_factory_h
#define sdl_music_factory_h

#include "SDLMusic/SDLMusic.h"

#include "engine/include/IMusicFactory/IMusicFactory.h"

#include <memory>
#include <string>

class SDLMusicFactory : public IMusicFactory
{
public:
    std::shared_ptr<Music> createMusic(const std::string& path) override;
};

#endif // sdl_music_factory_h
