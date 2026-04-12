#ifndef renderer_h
#define renderer_h

#include "../Texture/Texture.h"
#include "../Viewport/Viewport.h"

#include <memory>
#include <string>

class Renderer
{
public:
    struct DrawParams
    {
        int x = 0, y = 0, width = 0, height = 0;
        float rotation = 0.0f;
        float pivotX = 0.5f, pivotY = 0.5f;
        bool flipX = false, flipY = false;
        int srcX = 0, srcY = 0, srcWidth = 0, srcHeight = 0;
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
