#ifndef stub_texture_h
#define stub_texture_h

#include "engine/resources/Texture/Texture.h"

class StubTexture : public Texture
{
public:
    int getWidth() const override { return 64; }
    int getHeight() const override { return 96; }
};

#endif // stub_texture_h
