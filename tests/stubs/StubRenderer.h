#ifndef stub_renderer_h
#define stub_renderer_h

#include "engine/include/Renderer/Renderer.h"

#include <vector>

class StubRenderer : public Renderer
{
public:
    struct Calls
    {
        int clear = 0, present = 0, setViewport = 0;
        int drawTexture = 0, drawFont = 0;
        int drawRectangle = 0, drawRectFilled = 0, drawRectOutline = 0;
        int drawCircle = 0, drawCircleFilled = 0, drawCircleOutline = 0;
        int setScale = 0;
    };

    struct LastDraw
    {
        int x = 0, y = 0;
        int width = 0, height = 0;
        float rotation = 0.f;
        bool flipX = false, flipY = false;
        int srcX = 0, srcY = 0;
        int srcWidth = 0, srcHeight = 0;
        bool useSourceRect = false;
    };

    StubRenderer()
    {
        this->registerHandler<DrawTextureCommand>([this]
            (const DrawTextureCommand& cmd) { this->onDrawTexture(cmd); });
        this->registerHandler<DrawFontCommand>([this]
            (const DrawFontCommand& cmd) { this->onDrawFont(cmd); });
        this->registerHandler<DrawRectangleCommand>([this]
            (const DrawRectangleCommand& cmd) { this->onDrawRectangle(cmd); });
        this->registerHandler<DrawCircleCommand>([this]
            (const DrawCircleCommand& cmd) { this->onDrawCircle(cmd); });
    }

    void clear() override
    {
        ++this->calls.clear;
        this->textureCalls.clear();
        this->fontCalls.clear();
        this->rectangleCalls.clear();
        this->circleCalls.clear();
        this->viewportHistory.clear();
        this->scaleHistory.clear();
        this->calls = Calls{};
        this->calls.clear = 1;
    }

    void present() override { ++this->calls.present; }

    void setViewport(const Viewport& vp) override
    {
        ++this->calls.setViewport;
        this->lastViewport = vp;
        this->viewportHistory.push_back(vp);
        this->currentViewport = vp;
    }

    void setScale(const Position& scale) override
    {
        ++this->calls.setScale;
        this->lastScale = scale;
        this->scaleHistory.push_back(scale);
    }

    Calls calls;
    LastDraw lastDraw;
    Rectangle lastRect;
    Circle lastCircle;
    Color lastColor;
    Viewport lastViewport, currentViewport;
    Position lastScale;

    std::vector<DrawTextureCommand> textureCalls;
    std::vector<DrawFontCommand> fontCalls;
    std::vector<DrawRectangleCommand> rectangleCalls;
    std::vector<DrawCircleCommand> circleCalls;
    std::vector<Viewport> viewportHistory;
    std::vector<Position> scaleHistory;

private:
    void onDrawTexture(const DrawTextureCommand& cmd)
    {
        ++this->calls.drawTexture;
        this->textureCalls.push_back(cmd);
        this->lastDraw.x = static_cast<int>(cmd.dest.position.x);
        this->lastDraw.y = static_cast<int>(cmd.dest.position.y);
        this->lastDraw.width = static_cast<int>(cmd.dest.size.width);
        this->lastDraw.height = static_cast<int>(cmd.dest.size.height);
        this->lastDraw.rotation = cmd.rotation;
        this->lastDraw.flipX = cmd.flipX;
        this->lastDraw.flipY = cmd.flipY;
        this->lastDraw.srcX = static_cast<int>(cmd.source.position.x);
        this->lastDraw.srcY = static_cast<int>(cmd.source.position.y);
        this->lastDraw.srcWidth = static_cast<int>(cmd.source.size.width);
        this->lastDraw.srcHeight = static_cast<int>(cmd.source.size.height);
        this->lastDraw.useSourceRect = cmd.useSourceRect;
    }

    void onDrawFont(const DrawFontCommand& cmd)
    {
        ++this->calls.drawFont;
        this->fontCalls.push_back(cmd);
    }

    void onDrawRectangle(const DrawRectangleCommand& cmd)
    {
        ++this->calls.drawRectangle;
        if (cmd.filled) ++this->calls.drawRectFilled;
        else ++this->calls.drawRectOutline;
        this->rectangleCalls.push_back(cmd);
        this->lastRect = cmd.rect;
        this->lastColor = cmd.color;
    }

    void onDrawCircle(const DrawCircleCommand& cmd)
    {
        ++this->calls.drawCircle;
        if (cmd.filled) ++this->calls.drawCircleFilled;
        else ++this->calls.drawCircleOutline;
        this->circleCalls.push_back(cmd);
        this->lastCircle = cmd.circle;
        this->lastColor = cmd.color;
    }
};

#endif // stub_renderer_h
