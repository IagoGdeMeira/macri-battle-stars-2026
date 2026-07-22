#ifndef draw_circle_batch_h
#define draw_circle_batch_h

#include "DrawBatch/DrawBatch.h"
#include "DrawCircleCommand.h"
#include "Renderer/Renderer.h"

class DrawCircleBatch : public DrawBatch<DrawCircleCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : this->commands) renderer.draw(cmd);
    }
};

#endif // draw_circle_batch_h
