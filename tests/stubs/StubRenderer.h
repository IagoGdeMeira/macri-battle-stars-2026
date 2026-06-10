#ifndef stub_renderer_h
#define stub_renderer_h

#include "../../src/engine/include/Renderer/Renderer.h"
#include <vector>

class StubRenderer : public Renderer
{
public:
    struct Calls
    {
        int clear = 0, present = 0;
        int drawTexture = 0, drawFont = 0;
        int drawRectangle = 0, drawRectFilled = 0, drawRectOutline = 0;
        int drawCircle = 0, drawCircleFilled = 0, drawCircleOutline = 0;
        int setViewport = 0;
    };

    struct LastDraw
    {
        int x = 0, y = 0, width = 0, height = 0;
        float rotation = 0.f;
        bool flipX = false, flipY = false;
        int srcX = 0, srcY = 0, srcWidth = 0, srcHeight = 0;
        bool useSourceRect = false;
    };

    Calls calls;
    Viewport currentViewport;

    LastDraw lastDraw;
    Rectangle lastRect;
    Circle lastCircle;
    Color lastColor;
    Viewport lastViewport;

    std::vector<DrawTextureCommand> textureCalls;
    std::vector<DrawFontCommand> fontCalls;
    std::vector<DrawRectangleCommand> rectangleCalls;
    std::vector<DrawCircleCommand> circleCalls;
    std::vector<Viewport> viewportHistory;

    void clear() override
    {
        this->textureCalls.clear();
        this->fontCalls.clear();
        this->rectangleCalls.clear();
        this->circleCalls.clear();
        this->calls.setViewport = 0;
        this->calls.clear = 0;
        this->calls.present = 0;
    }

    void present() override { ++this->calls.present; }

    void drawTexture(const DrawTextureCommand& cmd) override
    {
        ++this->calls.drawTexture;
        this->textureCalls.push_back(cmd);
        this->lastDraw.x                = static_cast<int>(cmd.dest.position.x);
        this->lastDraw.y                = static_cast<int>(cmd.dest.position.y);
        this->lastDraw.width            = static_cast<int>(cmd.dest.size.width);
        this->lastDraw.height           = static_cast<int>(cmd.dest.size.height);
        this->lastDraw.rotation         = cmd.rotation;
        this->lastDraw.flipX            = cmd.flipX;
        this->lastDraw.flipY            = cmd.flipY;
        this->lastDraw.srcX             = static_cast<int>(cmd.source.position.x);
        this->lastDraw.srcY             = static_cast<int>(cmd.source.position.y);
        this->lastDraw.srcWidth         = static_cast<int>(cmd.source.size.width);
        this->lastDraw.srcHeight        = static_cast<int>(cmd.source.size.height);
        this->lastDraw.useSourceRect    = cmd.useSourceRect;
    }

    void drawFont(const DrawFontCommand& cmd) override { (void)cmd; ++this->calls.drawFont; this->fontCalls.push_back(cmd); }

    void drawRectangle(const DrawRectangleCommand& cmd) override
    {
        ++this->calls.drawRectangle;
        if (cmd.filled) ++this->calls.drawRectFilled;
        else ++this->calls.drawRectOutline;
        this->rectangleCalls.push_back(cmd);
        this->lastRect = cmd.rect;
        this->lastColor = cmd.color;
    }

    void drawCircle(const DrawCircleCommand& cmd) override
    {
        ++this->calls.drawCircle;
        if (cmd.filled) ++this->calls.drawCircleFilled;
        else ++this->calls.drawCircleOutline;
        this->circleCalls.push_back(cmd);
        this->lastCircle = cmd.circle;
        this->lastColor = cmd.color;
    }

    void setViewport(const Viewport& vp) override
    {
        ++this->calls.setViewport;
        this->lastViewport = vp;
        this->viewportHistory.push_back(vp);
        this->currentViewport = vp;
    }
};

#endif // stub_renderer_h
