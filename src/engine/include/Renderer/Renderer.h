#ifndef renderer_h
#define renderer_h

#include "IDrawCommandHandler/IDrawCommandHandler.h"
#include "Viewport/Viewport.h"

#include "domain/value_objects/Geometry/Geometry.h"

#include <unordered_map>
#include <typeindex>
#include <memory>

class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;
    virtual void setViewport(const Viewport&) = 0;
    virtual void setScale(const Position&) = 0;

    template<typename CommandType>
    void registerHandler(std::unique_ptr<IDrawCommandHandler> handler)
    { this->handlers[std::type_index(typeid(CommandType))] = std::move(handler); }

    virtual void draw(const DrawCommand& command)
    {
        auto it = this->handlers.find(command.type());
        if (it != this->handlers.end()) it->second->execute(command);
    }

protected:
    std::unordered_map<std::type_index, std::unique_ptr<IDrawCommandHandler>> handlers;
};

#endif // renderer_h
