#ifndef stub_renderer_h
#define stub_renderer_h

#include "engine/include/Renderer/Renderer.h"

#include "engine/draw_commands/DrawTextureCommand.h"
#include "engine/draw_commands/DrawFontCommand.h"
#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/draw_commands/DrawCircleCommand.h"

#include <vector>
#include <typeinfo>

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

    void draw(const DrawCommand& command) override
    {
        if (const auto* texCmd = dynamic_cast<const DrawTextureCommand*>(&command))
        {
            ++this->calls.drawTexture;
            this->textureCalls.push_back(*texCmd);
            this->lastDraw.x = static_cast<int>(texCmd->dest.position.x);
            this->lastDraw.y = static_cast<int>(texCmd->dest.position.y);
            this->lastDraw.width = static_cast<int>(texCmd->dest.size.width);
            this->lastDraw.height = static_cast<int>(texCmd->dest.size.height);
            this->lastDraw.rotation = texCmd->rotation;
            this->lastDraw.flipX = texCmd->flipX;
            this->lastDraw.flipY = texCmd->flipY;
            this->lastDraw.srcX = static_cast<int>(texCmd->source.position.x);
            this->lastDraw.srcY = static_cast<int>(texCmd->source.position.y);
            this->lastDraw.srcWidth = static_cast<int>(texCmd->source.size.width);
            this->lastDraw.srcHeight = static_cast<int>(texCmd->source.size.height);
            this->lastDraw.useSourceRect = texCmd->useSourceRect;
        }
        else if (const auto* fontCmd = dynamic_cast<const DrawFontCommand*>(&command))
        {
            ++this->calls.drawFont;
            this->fontCalls.push_back(*fontCmd);
        }
        else if (const auto* rectCmd = dynamic_cast<const DrawRectangleCommand*>(&command))
        {
            ++this->calls.drawRectangle;
            if (rectCmd->filled) ++this->calls.drawRectFilled;
            else ++this->calls.drawRectOutline;
            this->rectangleCalls.push_back(*rectCmd);
            this->lastRect = rectCmd->rect;
            this->lastColor = rectCmd->color;
        }
        else if (const auto* circleCmd = dynamic_cast<const DrawCircleCommand*>(&command))
        {
            ++this->calls.drawCircle;
            if (circleCmd->filled) ++this->calls.drawCircleFilled;
            else ++this->calls.drawCircleOutline;
            this->circleCalls.push_back(*circleCmd);
            this->lastCircle = circleCmd->circle;
            this->lastColor = circleCmd->color;
        }
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
};

#endif // stub_renderer_h 
