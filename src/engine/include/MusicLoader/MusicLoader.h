#ifndef music_loader_h
#define music_loader_h

#include "../IMusicFactory/IMusicFactory.h"
#include "../Music/Music.h"
#include "../ResourceLoader/ResourceLoader.h"

class MusicLoader : public ResourceLoader<Music>
{
public:
    explicit MusicLoader(IMusicFactory& factory) : factory(factory) {}

    std::shared_ptr<Music> load(const std::string& path) override
    { return this->factory.createMusic(path); }

private:
    IMusicFactory& factory;
};

#endif // music_loader_h
