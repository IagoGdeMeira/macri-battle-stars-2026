#ifndef renderer_h
#define renderer_h

#include "../DrawCommands/DrawCommands.h"
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
    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;

    virtual void drawTexture(const DrawTextureCommand& cmd) = 0;
    virtual void drawFont(const DrawFontCommand& cmd) = 0;
    virtual void drawRectangle(const DrawRectangleCommand& cmd) = 0;
    virtual void drawCircle(const DrawCircleCommand& cmd) = 0;

    virtual void setViewport(const Viewport& viewport) = 0;
};

#endif // renderer_h
