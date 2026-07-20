#ifndef i_music_factory_h
#define i_music_factory_h

#include "Music/Music.h"

#include <memory>
#include <string>

class IMusicFactory
{
public:
    virtual ~IMusicFactory() = default;
    virtual std::shared_ptr<Music> createMusic(const std::string& path) = 0;
};

#endif // i_music_factory_h
