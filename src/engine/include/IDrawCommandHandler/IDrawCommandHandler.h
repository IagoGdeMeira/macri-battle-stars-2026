#ifndef i_draw_command_handler_h
#define i_draw_command_handler_h

#include "DrawCommand/DrawCommand.h"

class IDrawCommandHandler
{
public:
    virtual ~IDrawCommandHandler() = default;
    virtual void execute(const DrawCommand& command) = 0;
};

#endif // i_draw_command_handler_h
