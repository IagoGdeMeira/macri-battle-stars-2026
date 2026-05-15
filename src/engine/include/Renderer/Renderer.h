#ifndef renderer_h
#define renderer_h

#include "../Font/Font.h"
#include "../Texture/Texture.h"
#include "../Viewport/Viewport.h"

#include "../../domain/include/Color/Color.h"
#include "../../domain/include/Geometry/Geometry.h"

#include <cstdint>
#include <memory>
#include <string>

class Renderer
{
public:
    struct DrawTextureParams
    {
        Rectangle dest;
        float rotation = 0.0f;
        float pivotX = 0.5f, pivotY = 0.5f;
        bool flipX = false, flipY = false;
        Rectangle source;
        bool useSourceRect = false;
    };

    struct DrawTextParams
    {
        Rectangle dest;
        int fontSize = 16;
        Color color = {255, 255, 255, 255};
    };

    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;

    virtual void drawTexture(const Texture& texture, const DrawTextureParams& params) = 0;
    virtual void drawText(const Font& font, std::string text, const DrawTextParams& params) = 0;

    virtual void drawRectOutline(const Rectangle& rect, const Color& color) = 0;
    virtual void drawRectFilled(const Rectangle& rect, const Color& color) = 0;
    virtual void drawCircleOutline(const Circle& circle, const Color& color) = 0;
    virtual void drawCircleFilled(const Circle& circle, const Color& color) = 0;

    virtual void setViewport(const Viewport& viewport) = 0;
};

#endif // renderer_h
