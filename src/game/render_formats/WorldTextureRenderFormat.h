#ifndef world_texture_render_format_h
#define world_texture_render_format_h

#include "../include/Camera2D/Camera2D.h"
#include "../include/IRenderFormat/IRenderFormat.h"

#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VisualEffectsComponent.h"
#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/DrawBatch/DrawTextureBatch.h"
#include "../../engine/include/DrawCommands/DrawCommands.h"
#include "../../engine/include/RenderContext/RenderContext.h"
#include "../../engine/include/Renderer/Renderer.h"

class WorldTextureRenderFormat : public IRenderFormat
{
public:
    WorldTextureRenderFormat(Renderer& renderer, Camera2D& camera) :
        renderer(renderer), camera(camera) {}

    void render(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Camera2D& camera;
    DrawTextureBatch batch;

    DrawTextureCommand buildTextureCommand(Entity& entity, World& world, size_t order) const;
    Position worldToScreen(Position worldPos, const Viewport& viewport, Position parallax = {1.f, 1.f}) const;
    Position resolveParallax(World& world, Entity entity) const;
    void computeSpriteTransform(const Rectangle& spriteConfig, DrawTextureCommand& cmd) const;
    Viewport getViewport(RenderContext& ctx) const;
};

#endif // world_texture_render_format_h
