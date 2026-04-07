#ifndef render_system_h
#define render_system_h

#include "../System/System.h"
#include "../Renderer/Renderer.h"

#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"

class RenderSystem : public System
{
public:
    RenderSystem(Renderer& renderer) : renderer(renderer) {}

    void update(UpdateContext& ctx) override;

private:
    Renderer& renderer;
};

#endif // render_system_h
