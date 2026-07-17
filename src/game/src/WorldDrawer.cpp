#include "../include/WorldDrawer/WorldDrawer.h"

#include "../render_formats/WorldCircleRenderFormat.h"
#include "../render_formats/WorldRectangleRenderFormat.h"
#include "../render_formats/WorldTextureRenderFormat.h"

#include "../../domain/utils/Logger/Logger.h"

#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"

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
    this->renderer.setViewport(this->worldViewport);
    for (auto& format : this->formats) format->render(ctx);
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
