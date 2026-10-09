#include "WorldDrawer/WorldDrawer.h"

#include "WorldCircleRenderFormat.h"
#include "WorldRectangleRenderFormat.h"
#include "WorldTextureRenderFormat.h"

#include "engine/events/WindowResizedEvent.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

WorldDrawer::WorldDrawer(Config&& cfg) : cfg(std::move(cfg))
{
    this->recalculateViewport();

    cfg.bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent&) { this->recalculateViewport(); });

    this->addFormat(std::make_unique<WorldRectangleRenderFormat>(cfg.renderer, cfg.camera));
    this->addFormat(std::make_unique<WorldCircleRenderFormat>(cfg.renderer, cfg.camera));
    this->addFormat(std::make_unique<WorldTextureRenderFormat>(WorldTextureRenderFormat::Config{
        .renderer           = this->cfg.renderer,
        .camera             = this->cfg.camera,
        .resourceManager    = this->cfg.resourceManager,
        .textureLoader      = this->cfg.textureLoader
    }));
}

void WorldDrawer::addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }

void WorldDrawer::draw(RenderContext& ctx)
{
    float viewportScale = static_cast<float>(this->worldViewport.width) / this->vSize.width;
    float totalScale = viewportScale;

    this->cfg.renderer.setViewport(this->worldViewport);
    this->cfg.renderer.setScale(Position{totalScale, totalScale});

    RenderQueue queue;
    for (auto& format : this->formats) format->render(ctx, queue);
    queue.submit(this->cfg.renderer);

    this->cfg.renderer.setScale(Position{1.f, 1.f});
}

void WorldDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->cfg.settings.screen.size;

    const float scale = std::min(winSize.width / this->vSize.width, winSize.height / this->vSize.height);
    const Dimension2D viewSize { this->vSize.width * scale, this->vSize.height * scale };
    const Position offset {(winSize.width - viewSize.width) * 0.5f, (winSize.height - viewSize.height) * 0.5f};

    this->worldViewport = {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}
