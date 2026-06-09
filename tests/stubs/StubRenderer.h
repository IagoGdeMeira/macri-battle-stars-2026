#ifndef stub_renderer_h
#define stub_renderer_h

#include "../../src/engine/include/Renderer/Renderer.h"

class StubRenderer : public Renderer
{
public:
    void clear() override {}
    void present() override {}
    void drawTexture(const DrawTextureCommand&) override {}
    void drawFont(const DrawFontCommand&) override {}
    void drawCircle(const DrawCircleCommand&) override {}
    void drawRectangle(const DrawRectangleCommand&) override {}
    void setViewport(const Viewport&) override {}
};

#endif // stub_renderer_h
