#ifndef stub_texture_factory_h
#define stub_texture_factory_h

#include "StubTexture.h"

#include "engine/include/ITextureFactory/ITextureFactory.h"

class StubTextureFactory : public ITextureFactory
{
public:
    int createTextureCalls = 0;
    std::string lastPath;

    std::shared_ptr<Texture> createTexture(const std::string& path) override
    {
        ++this->createTextureCalls;
        this->lastPath = path;
        if (path.empty()) return nullptr;
        return std::make_shared<StubTexture>();
    }
};

#endif // stub_texture_factory_h
