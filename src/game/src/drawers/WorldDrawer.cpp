#include "WorldDrawer/WorldDrawer.h"

#include "WorldCircleRenderFormat.h"
#include "WorldRectangleRenderFormat.h"
#include "WorldTextureRenderFormat.h"

#include "domain/utils/Logger/Logger.h"

#include "engine/events/WindowResizedEvent.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"

WorldDrawer::WorldDrawer(Config&& cfg) :
    renderer(cfg.renderer),
    camera(cfg.camera),
    settings(cfg.settings),
    resourceManager(cfg.resourceManager),
    textureLoader(cfg.textureLoader)
{
    this->recalculateViewport();

    cfg.bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent&)
    {
        this->recalculateViewport();
        this->propagateViewport();
    });

    this->addFormat(std::make_unique<WorldTextureRenderFormat>(WorldTextureRenderFormat::Config{
        .renderer           = this->renderer,
        .camera             = this->camera,
        .resourceManager    = this->resourceManager,
        .textureLoader      = this->textureLoader
    }));

    this->addFormat(std::make_unique<WorldRectangleRenderFormat>(renderer, camera));
    this->addFormat(std::make_unique<WorldCircleRenderFormat>(renderer, camera));

    this->propagateViewport();
}

void WorldDrawer::addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }

void WorldDrawer::draw(RenderContext& ctx)
{
    float zoom = this->camera.getZoom();
    LOG_DEBUG("WorldDrawer::draw: start, zoom={}", zoom);

    this->renderer.setScale(Position{zoom, zoom});
    LOG_DEBUG("WorldDrawer::draw: after setScale");

    this->renderer.setViewport(this->worldViewport);
    LOG_DEBUG("WorldDrawer::draw: viewport set to ({},{},{},{})",
        this->worldViewport.x, this->worldViewport.y,
        this->worldViewport.width, this->worldViewport.height);

    for (auto& format : this->formats)
    {
        LOG_DEBUG("WorldDrawer::draw: rendering format");
        format->render(ctx);
    }

    this->renderer.setScale(Position{1.f, 1.f});
    LOG_DEBUG("WorldDrawer::draw: end");
}
void WorldDrawer::recalculateViewport()
{
    const Dimension2D& winSize = this->settings.screen.size;

    const auto& vWidth = this->vSize.width;
    const auto& vHeight = this->vSize.height;
    const float scale = std::min(winSize.width / vWidth, winSize.height / vHeight);

    const Dimension2D& viewSize = {vWidth * scale, vHeight * scale};
    const Position offset = {(winSize.width - viewSize.width) * 0.5f, (winSize.height - viewSize.height) * 0.5f};

    this->worldViewport =
    {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(viewSize.width), static_cast<int>(viewSize.height)
    };
}

void WorldDrawer::propagateViewport() { for (auto& format : this->formats) format->setViewport(this->worldViewport); }
