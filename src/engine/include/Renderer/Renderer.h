#ifndef renderer_h
#define renderer_h

#include "DrawCommands/DrawCommands.h"
#include "Viewport/Viewport.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;
    virtual void setViewport(const Viewport& viewport) = 0;
    virtual void setScale(const Position& scale) = 0;

    void draw(const DrawCommand& command)
    {
        auto it = this->handlers.find(command.type());
        if (it != this->handlers.end()) it->second(command);
    }

protected:
    template <typename Command>
    void registerHandler(std::function<void(const Command&)> handler)
    {
        this->handlers[Command().type()] = [handler](const DrawCommand& cmd)
        { handler(static_cast<const Command&>(cmd)); };
    }

private:
    using HandlerFunc = std::function<void(const DrawCommand&)>;
    std::unordered_map<DrawCommand::Type, HandlerFunc> handlers;
};

#endif // renderer_h
