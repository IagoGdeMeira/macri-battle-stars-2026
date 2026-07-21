#ifndef world_drawer_h
#define world_drawer_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/Drawer/Drawer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/Camera2D/Camera2D.h"
#include "engine/value_objects/GameConstants/GameConstants.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/Viewport/Viewport.h"

#include <memory>
#include <vector>

class ResourceManager;
class TextureLoader;

class WorldDrawer : public Drawer
{
public:
    struct Config
    {
        EventBus& bus;
        Renderer& renderer;
        Camera2D& camera;
        GameSettings& settings;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
    };

    explicit WorldDrawer(Config&& cfg);
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Camera2D& camera;
    GameSettings& settings;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;

    const Dimension2D vSize = GameConstants::VIRTUAL_SIZE;

    std::vector<std::unique_ptr<IRenderFormat>> formats;
    Viewport worldViewport {0, 0, static_cast<int>(this->vSize.width), static_cast<int>(this->vSize.height)};

    void addFormat(std::unique_ptr<IRenderFormat> format);
    void recalculateViewport();
    void propagateViewport();
};

#endif // world_drawer_h
