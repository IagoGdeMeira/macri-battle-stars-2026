#ifndef sdl_renderer_h
#define sdl_renderer_h

#include "SDLDrawCircleCommandHandler.h"
#include "SDLDrawFontCommandHandler.h"
#include "SDLDrawRectangleCommandHandler.h"
#include "SDLDrawTextureCommandHandler.h"

#include "engine/include/Renderer/Renderer.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <memory>
#include <SDL.h>
#include <typeindex>
#include <unordered_map>

class SDLRenderer : public Renderer
{
public:
    explicit SDLRenderer(SDL_Window* window);
    ~SDLRenderer() override;

    void clear() override;
    void present() override;
    void setViewport(const Viewport& viewport) override;
    void setScale(const Position& scale) override;
    void draw(const DrawCommand& command) override;

    SDL_Renderer* get() const { return this->renderer; }

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::type_index, std::unique_ptr<IDrawCommandHandler>> handlers;

    template <typename CommandType>
    void registerHandler(std::unique_ptr<IDrawCommandHandler> handler)
    { this->handlers[std::type_index(typeid(CommandType))] = std::move(handler); }
};

#endif // sdl_renderer_h
