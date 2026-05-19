#ifndef draw_circle_batch_h
#define draw_circle_batch_h

#include "DrawBatch.h"

#include "../DrawCommands/DrawCommands.h"
#include "../Renderer/Renderer.h"

class DrawCircleBatch : public DrawBatch<DrawCircleCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : commands) renderer.drawCircle(cmd);
    }
};

#endif // draw_circle_batch_h
