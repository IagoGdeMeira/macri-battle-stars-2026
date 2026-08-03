#ifndef draw_command_h
#define draw_command_h

#include <tuple>
#include <typeindex>
#include <cstddef>

class DrawCommand
{
public:
    int layer = 0, zIndex = 0;
    std::size_t order = 0;

    virtual ~DrawCommand() = default;
    virtual std::type_index type() const = 0;

    std::tuple<int, int, std::size_t> getSortKey() const { return {this->layer, this->zIndex, this->order}; }
};

#endif // draw_command_h
