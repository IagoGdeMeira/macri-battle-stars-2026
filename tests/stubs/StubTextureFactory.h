#ifndef stub_texture_factory_h
#define stub_texture_factory_h

#include "StubTexture.h"

#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"

#include <memory>
#include <string>

class StubTextureFactory : public ITextureFactory
{
public:
    std::shared_ptr<Texture> textureToReturn = std::make_shared<StubTexture>();
    std::shared_ptr<Texture> createTexture(const std::string&) override { return this->textureToReturn; }
};

#endif // stub_texture_factory_h
