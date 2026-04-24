#ifndef renderer_h
#define renderer_h

#include "../GeometryUtils/GeometryUtils.h"
#include "../Texture/Texture.h"
#include "../Viewport/Viewport.h"

#include <cstdint>
#include <memory>
#include <string>

class Renderer
{
public:
    struct Color { std::uint8_t r = 255, g = 255, b = 255, a = 255; };

    struct DrawParams
    {
        Rectangle dest;
        float rotation = 0.0f;
        float pivotX = 0.5f, pivotY = 0.5f;
        bool flipX = false, flipY = false;
        Rectangle source;
        bool useSourceRect = false;
    };

    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;
    
    virtual std::shared_ptr<Texture> createTexture(const std::string& filePath) = 0;

    virtual void draw(const Texture& texture, const DrawParams& params) = 0;

    virtual void setViewport(const Viewport& viewport) = 0;
};

#endif // renderer_h
