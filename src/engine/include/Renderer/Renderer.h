#ifndef renderer_h
#define renderer_h

#include "../Texture/Texture.h"

#include <memory>
#include <string>

class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;
    
    virtual std::shared_ptr<Texture> createTexture(const std::string& filePath) = 0;

    virtual void draw(
        const Texture& texture,
        int x, int y, int width, int height
    ) = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;
};

#endif // renderer_h
