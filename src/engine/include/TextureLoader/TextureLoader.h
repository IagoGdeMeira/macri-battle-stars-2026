#ifndef texture_loader_h
#define texture_loader_h

#include "ITextureFactory/ITextureFactory.h"
#include "ResourceLoader/ResourceLoader.h"

#include "domain/resources/Texture/Texture.h"

class TextureLoader : public ResourceLoader<Texture>
{
public:
    explicit TextureLoader(ITextureFactory& factory) : factory(factory) {}

    std::shared_ptr<Texture> load(const std::string& path) override
    { return this->factory.createTexture(path); }

private:
    ITextureFactory& factory;
};

#endif // texture_loader_h
