#ifndef draw_rectangle_batch_h
#define draw_rectangle_batch_h

#include "DrawBatch/DrawBatch.h"
#include "DrawRectangleCommand.h"
#include "Renderer/Renderer.h"

class DrawRectangleBatch : public DrawBatch<DrawRectangleCommand>
{
public:
    void submit(Renderer& renderer) override
    {
        this->sort();
        for (const auto& cmd : this->commands) renderer.draw(cmd);
    }
};

#endif // draw_rectangle_batch_h
