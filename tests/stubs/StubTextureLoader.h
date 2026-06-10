#ifndef stub_texture_loader_h
#define stub_texture_loader_h

#include "StubTextureFactory.h"

#include "../../src/engine/include/TextureLoader/TextureLoader.h"

class StubTextureLoader : public TextureLoader
{
public:
    explicit StubTextureLoader(ITextureFactory& factory) : TextureLoader(factory) {}
};

#endif // stub_texture_loader_h
