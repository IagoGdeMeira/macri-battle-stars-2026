#ifndef i_sound_factory_h
#define i_sound_factory_h

#include "Sound/Sound.h"

#include <memory>
#include <string>

class ISoundFactory
{
public:
    virtual ~ISoundFactory() = default;
    virtual std::shared_ptr<Sound> createSound(const std::string& path) = 0;
};

#endif // i_sound_factory_h
