#ifndef sdl_sound_factory_h
#define sdl_sound_factory_h

#include "../SDLSound/SDLSound.h"

#include "../../engine/include/ISoundFactory/ISoundFactory.h"

#include <memory>
#include <string>

class SDLSoundFactory : public ISoundFactory
{
public:
    std::shared_ptr<Sound> createSound(const std::string& path) override;
};

#endif // sdl_sound_factory_h
