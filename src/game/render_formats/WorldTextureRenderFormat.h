#ifndef world_texture_render_format_h
#define world_texture_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawTextureCommand.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/value_objects/Camera2D/Camera2D.h"

class WorldTextureRenderFormat : public IRenderFormat
{
public:
    struct Config
    {
        Renderer& renderer;
        Camera2D& camera;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
    };

    WorldTextureRenderFormat(Config&& cfg) :
        renderer(cfg.renderer),
        camera(cfg.camera),
        resourceManager(cfg.resourceManager),
        textureLoader(cfg.textureLoader) {}

    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    Camera2D& camera;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;

    DrawTextureCommand buildTextureCommand(Entity& entity, World& world, size_t order, std::shared_ptr<Texture> texture) const;
};

#endif // world_texture_render_format_h
