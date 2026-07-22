#ifndef draw_command_h
#define draw_command_h

#include <typeindex>

class DrawCommand
{
public:
    virtual ~DrawCommand() = default;
    virtual std::type_index type() const = 0;
};

#endif // draw_command_h
