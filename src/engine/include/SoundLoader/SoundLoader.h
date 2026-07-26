#ifndef sound_loader_h
#define sound_loader_h

#include "ISoundFactory/ISoundFactory.h"
#include "ResourceLoader/ResourceLoader.h"

#include "domain/resources/Sound/Sound.h"

class SoundLoader : public ResourceLoader<Sound>
{
public:
    explicit SoundLoader(ISoundFactory& factory) : factory(factory) {}
    
    std::shared_ptr<Sound> load(const std::string& path) override
    { return this->factory.createSound(path); }

private:
    ISoundFactory& factory;
};

#endif // sound_loader_h
