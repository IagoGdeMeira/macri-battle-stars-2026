#ifndef stub_renderer_h
#define stub_renderer_h

#include "../../src/engine/include/Renderer/Renderer.h"

#include <vector>

class StubRenderer : public Renderer
{
public:
    void clear() override
    {
        this->textureCalls.clear();
        this->fontCalls.clear();
        this->rectangleCalls.clear();
        this->circleCalls.clear();
    }

    void present() override {}

    void drawTexture(const DrawTextureCommand& cmd) override { this->textureCalls.push_back(cmd); }
    void drawFont(const DrawFontCommand& cmd) override { this->fontCalls.push_back(cmd); }
    void drawRectangle(const DrawRectangleCommand& cmd) override { this->rectangleCalls.push_back(cmd); }
    void drawCircle(const DrawCircleCommand& cmd) override { this->circleCalls.push_back(cmd); }

    void setViewport(const Viewport&) override {}

    std::vector<DrawTextureCommand> textureCalls;
    std::vector<DrawFontCommand> fontCalls;
    std::vector<DrawRectangleCommand> rectangleCalls;
    std::vector<DrawCircleCommand> circleCalls;
};

#endif // stub_renderer_h
