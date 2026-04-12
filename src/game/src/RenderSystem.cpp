#include "../include/RenderSystem/RenderSystem.h"

#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

RenderSystem::RenderSystem(EventBus& bus, Renderer& renderer, Camera2D& camera) :
    renderer(renderer), camera(camera)
{
    this->updateViewports();

    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& event)
    {
        this->windowWidth = event.width;
        this->windowHeight = event.height;
        this->updateViewports();
    });
}

void RenderSystem::update(UpdateContext& ctx)
{
    this->renderer.clear();

    this->renderer.setViewport(this->worldViewport);
    this->renderWorld(ctx);

    this->renderer.setViewport(this->uiViewport);
    this->renderUI(ctx);

    this->renderer.present();
}

void RenderSystem::renderWorld(UpdateContext& ctx)
{
    std::vector<DrawCommand> commands;
    commands.reserve(128);

    auto view = View<TransformComponent, SpriteComponent, RenderComponent>(
        ctx.world.components());

    size_t orderCounter = 0;

    for (auto [entity, transform, sprite, render] : view)
    {
        if (!sprite.texture) continue;

        float parallaxX = 1.0f, parallaxY = 1.0f;
        this->resolveParallaxFactors(ctx, entity, parallaxX, parallaxY);

        float screenX, screenY;
        this->worldToScreen(
            transform.x, transform.y,
            screenX, screenY,
            this->worldViewport,
            parallaxX, parallaxY);

        int finalWidth = 0, finalHeight = 0;
        bool flipX = false, flipY = false;
        this->resolveScaleAndFlip(
            sprite.width, sprite.height,
            transform.scaleX, transform.scaleY,
            finalWidth, finalHeight,
            flipX, flipY);

        commands.push_back(DrawCommand{
            sprite.texture.get(),
            static_cast<int>(screenX), static_cast<int>(screenY),
            finalWidth, finalHeight,
            transform.rotation,
            flipX, flipY,
            render.layer, render.zIndex,
            orderCounter,
            sprite.srcX, sprite.srcY, sprite.srcWidth, sprite.srcHeight,
            sprite.useSourceRect
        });
        orderCounter++;
    }

    this->sortDrawCommands(commands);
    this->submitDrawCommands(commands);
}

void RenderSystem::renderUI(UpdateContext& ctx)
{
    (void)ctx;
    // TODO: Implement UI rendering
    // This function will handle rendering UI elements on top of the game world
    // Something like: View<UISpriteComponent, UITransformComponent, RenderComponent>
};

void RenderSystem::worldToScreen(
    float worldX, float worldY,
    float& screenX, float& screenY,
    const Viewport& viewport,
    float parallaxX, float parallaxY
) {
    float camX = this->camera.getX();
    float camY = this->camera.getY();
    float zoom = this->camera.getZoom();

    screenX = (worldX - camX * parallaxX) * zoom + (viewport.width * 0.5f);
    screenY = (worldY - camY * parallaxY) * zoom + (viewport.height * 0.5f);
}

void RenderSystem::resolveParallaxFactors(
    UpdateContext& ctx, Entity entity,
    float& parallaxX, float& parallaxY
) const {
    try
    {
        if (!ctx.world.components().has<ParallaxComponent>(entity)) return;

        auto& parallax = ctx.world.components().get<ParallaxComponent>(entity);
        parallaxX = parallax.factorX;
        parallaxY = parallax.factorY;
    } catch (const std::logic_error&) {}
}

void RenderSystem::resolveScaleAndFlip(
    int spriteWidth, int spriteHeight,
    float scaleX, float scaleY,
    int& finalWidth, int& finalHeight,
    bool& flipX, bool& flipY
) const {
    flipX = scaleX < 0.0f;
    flipY = scaleY < 0.0f;

    const float zoom = this->camera.getZoom();
    finalWidth = static_cast<int>(spriteWidth * std::abs(scaleX) * zoom);
    finalHeight = static_cast<int>(spriteHeight * std::abs(scaleY) * zoom);
}

void RenderSystem::sortDrawCommands(std::vector<DrawCommand>& commands) const
{
    std::stable_sort(commands.begin(), commands.end(),
        [](const DrawCommand& a, const DrawCommand& b)
        {
            if (a.layer != b.layer) return a.layer < b.layer;
            if (a.zIndex != b.zIndex) return a.zIndex < b.zIndex;
            return a.order < b.order;
        });
}

void RenderSystem::submitDrawCommands(const std::vector<DrawCommand>& commands) const
{
    for (const auto& cmd : commands)
    {
        Renderer::DrawParams params;

        params.x = cmd.x;
        params.y = cmd.y;
        params.width = cmd.width;
        params.height = cmd.height;
        params.rotation = cmd.rotation;
        params.pivotX = 0.5f;
        params.pivotY = 0.5f;
        params.flipX = cmd.flipX;
        params.flipY = cmd.flipY;
        params.srcX = cmd.srcX;
        params.srcY = cmd.srcY;
        params.srcWidth = cmd.srcWidth;
        params.srcHeight = cmd.srcHeight;
        params.useSourceRect = cmd.useSourceRect;

        this->renderer.draw(*cmd.texture, params);
    }
}

void RenderSystem::updateViewports()
{
    float scaleX = (float)this->windowWidth / this->VIRTUAL_WIDTH;
    float scaleY = (float)this->windowHeight / this->VIRTUAL_HEIGHT;

    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    int viewWidth = (int)(this->VIRTUAL_WIDTH * scale);
    int viewHeight = (int)(this->VIRTUAL_HEIGHT * scale);

    int offsetX = (this->windowWidth - viewWidth) / 2;
    int offsetY = (this->windowHeight - viewHeight) / 2;

    this->worldViewport = { offsetX, offsetY, viewWidth, viewHeight};
    this->uiViewport = { 0, 0, this->windowWidth, this->windowHeight};
}
