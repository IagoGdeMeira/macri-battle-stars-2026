#ifndef i_texture_factory_h
#define i_texture_factory_h

#include "Texture/Texture.h"

#include <memory>
#include <string>

class ITextureFactory
{
public:
    virtual ~ITextureFactory() = default;
    virtual std::shared_ptr<Texture> createTexture(const std::string& path) = 0;
};

#endif // i_texture_factory_h
