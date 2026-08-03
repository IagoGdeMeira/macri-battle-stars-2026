#include "WorldDrawer/WorldDrawer.h"

#include "WorldCircleRenderFormat.h"
#include "WorldRectangleRenderFormat.h"
#include "WorldTextureRenderFormat.h"

#include "engine/events/WindowResizedEvent.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

#include <algorithm>

WorldDrawer::WorldDrawer(Config&& cfg) :
    renderer(cfg.renderer),
    camera(cfg.camera),
    settings(cfg.settings),
    resourceManager(cfg.resourceManager),
    textureLoader(cfg.textureLoader)
{
    this->recalculateViewport();

    cfg.bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent&) { this->recalculateViewport(); });

    this->addFormat(std::make_unique<WorldRectangleRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldCircleRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldTextureRenderFormat>(WorldTextureRenderFormat::Config{
        .renderer           = this->renderer,
        .camera             = this->camera,
        .resourceManager    = this->resourceManager,
        .textureLoader      = this->textureLoader
    }));
}

void WorldDrawer::addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }

void WorldDrawer::draw(RenderContext& ctx)
{
    float viewportScale = static_cast<float>(this->worldViewport.width) / this->vSize.width;
    float zoom = this->camera.getZoom();
    float totalScale = viewportScale * zoom;

    this->renderer.setViewport(this->worldViewport);
    this->renderer.setScale(Position{totalScale, totalScale});

    for (auto& format : this->formats) format->render(ctx);

    std::vector<const DrawCommand*> allCommands;
    for (auto& format : this->formats)
    {
        auto formatCmds = format->collectCommands();
        allCommands.insert(allCommands.end(), formatCmds.begin(), formatCmds.end());
    }

    std::stable_sort(allCommands.begin(), allCommands.end(), [](const DrawCommand* a, const DrawCommand* b)
    { return a->getSortKey() < b->getSortKey(); });

    for (auto* cmd : allCommands) this->renderer.draw(*cmd);

    this->renderer.setScale(Position{1.f, 1.f});
}

void WorldDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->settings.screen.size;

    const float scale = std::min(winSize.width / this->vSize.width, winSize.height / this->vSize.height);

    const Dimension2D viewSize { this->vSize.width * scale, this->vSize.height * scale };

    const Position offset {
        (winSize.width - viewSize.width) * 0.5f,
        (winSize.height - viewSize.height) * 0.5f
    };

    this->worldViewport = {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}
