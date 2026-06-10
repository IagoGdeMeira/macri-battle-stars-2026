#ifndef stub_texture_factory_h
#define stub_texture_factory_h

#include "StubTexture.h"

#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"

class StubTextureFactory : public ITextureFactory
{
public:
    int createTextureCalls = 0;
    std::string lastPath;
    std::shared_ptr<Texture> textureToReturn = std::make_shared<StubTexture>();

    std::shared_ptr<Texture> createTexture(const std::string& path) override
    {
        ++this->createTextureCalls;
        this->lastPath = path;
        return this->textureToReturn;
    }
};

#endif // stub_texture_factory_h
