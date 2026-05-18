#include "../include/WorldDrawer/WorldDrawer.h"

#include "../../domain/components/AnimationControllerComponent.h"
#include "../../domain/components/OrientationComponent.h"
#include "../../domain/components/ParallaxComponent.h"
#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/ShapeRenderComponent.h"
#include "../../domain/components/SpriteComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"
#include "../../engine/events/WindowResizedEvent.h"

#include <algorithm>
#include <cmath>

WorldDrawer::WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera)
    : renderer(renderer), camera(camera)
{
    this->updateViewports();
    bus.subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e)
    {
        this->windowWidth = e.width;
        this->windowHeight = e.height;
        this->updateViewports();
    });
}

void WorldDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(this->worldViewport);
    this->renderWorld(ctx);
    this->renderShapes(ctx);
}

void WorldDrawer::renderWorld(RenderContext& ctx)
{
    std::vector<Drawer::SpriteCommand> commands;
    commands.reserve(128);

    auto view = View<TransformComponent, SpriteComponent, RenderComponent>(ctx.world.components());
    size_t order = 0;
    for (auto [entity, transform, sprite, render] : view)
    {
        if (!sprite.texture) continue;
        commands.push_back(this->buildSpriteCommand(entity, ctx.world, order++));
    }

    this->sortCommands(commands);
    this->submitCommands(commands);
}

void WorldDrawer::renderShapes(RenderContext& ctx)
{
    auto view = View<TransformComponent, ShapeRenderComponent>(ctx.world.components());
    for (auto [entity, transform, shapeComp] : view)
    {
        if (!shapeComp.shape) continue;
        const Position screenPos = this->worldToScreen({transform.x, transform.y}, this->worldViewport);
        const float zoom = this->camera.getZoom();

        if (auto* rect = dynamic_cast<RectangleDef*>(shapeComp.shape.get()))
        {
            Rectangle r;
            r.position.x = screenPos.x - (rect->width  * std::abs(transform.scaleX) * zoom) * 0.5f;
            r.position.y = screenPos.y - (rect->height * std::abs(transform.scaleY) * zoom) * 0.5f;
            r.width = rect->width * std::abs(transform.scaleX) * zoom;
            r.height = rect->height * std::abs(transform.scaleY) * zoom;
            
            if (shapeComp.filled) this->renderer.drawRectFilled(r, shapeComp.color);
            else this->renderer.drawRectOutline(r, shapeComp.color);
        }
        else if (auto* circ = dynamic_cast<CircleDef*>(shapeComp.shape.get()))
        {
            Circle c;
            c.position = screenPos;
            c.radius = circ->radius * std::max(std::abs(transform.scaleX), std::abs(transform.scaleY)) * zoom;
            
            if (shapeComp.filled) this->renderer.drawCircleFilled(c, shapeComp.color);
            else this->renderer.drawCircleOutline(c, shapeComp.color);
        }
    }
}

Drawer::SpriteCommand WorldDrawer::buildSpriteCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& sprite = components.get<SpriteComponent>(entity);
    const auto& transform = components.get<TransformComponent>(entity);
    const auto& render = components.get<RenderComponent>(entity);

    const Position parallax = this->resolveParallax(world, entity);
    const Position screenPos = this->worldToScreen({transform.x, transform.y}, this->worldViewport, parallax);
    Rectangle spriteConfig = 
    {
        { transform.scaleX, transform.scaleY },
        static_cast<float>(sprite.width), static_cast<float>(sprite.height)
    };

    Drawer::SpriteCommand cmd;
    cmd.texture = sprite.texture.get();
    cmd.dest.position = screenPos;
    cmd.rotation = transform.rotation;

    this->computeSpriteTransform(spriteConfig, cmd);

    if (components.has<OrientationComponent>(entity))
    {
        bool symmetric = (components.has<AnimationControllerComponent>(entity))
            ? components.get<AnimationControllerComponent>(entity).animations.symmetric
            : true;

        if (symmetric)
        {
            const auto& orientation = components.get<OrientationComponent>(entity);
            cmd.flipX = (orientation.direction == Orientation::Left);
        }
        else cmd.flipX = false;
    }

    cmd.layer = render.layer;
    cmd.zIndex = render.zIndex;
    cmd.order = order;
    cmd.source.position.x = static_cast<float>(sprite.srcX);
    cmd.source.position.y = static_cast<float>(sprite.srcY);
    cmd.source.width = static_cast<float>(sprite.srcWidth);
    cmd.source.height = static_cast<float>(sprite.srcHeight);
    cmd.useSourceRect = sprite.useSourceRect;
    return cmd;
}

Position WorldDrawer::worldToScreen(Position worldPos, const Viewport& viewport, Position parallax) const
{
    const float camX = camera.getX();
    const float camY = camera.getY();
    const float zoom = camera.getZoom();
    
    return
    {
        (worldPos.x - camX * parallax.x) * zoom + viewport.width  * 0.5f,
        (worldPos.y - camY * parallax.y) * zoom + viewport.height * 0.5f
    };
}

Position WorldDrawer::resolveParallax(World& world, Entity entity) const
{
    Position p{1.0f, 1.0f};
    auto& components = world.components();
    if (components.has<ParallaxComponent>(entity))
    {
        auto& par = components.get<ParallaxComponent>(entity);
        p = { par.factorX, par.factorY };
    }
    return p;
}

void WorldDrawer::computeSpriteTransform(const Rectangle& spriteConfig, Drawer::SpriteCommand& cmd) const
{
    const bool flipX = spriteConfig.position.x < 0.0f;
    const bool flipY = spriteConfig.position.y < 0.0f;

    const float zoom = this->camera.getZoom();
    const int width = static_cast<int>(spriteConfig.width * std::abs(spriteConfig.position.x) * zoom);
    const int height = static_cast<int>(spriteConfig.height * std::abs(spriteConfig.position.y) * zoom);

    cmd.dest.width = static_cast<float>(width);
    cmd.dest.height = static_cast<float>(height);
    cmd.flipX = flipX;
    cmd.flipY = flipY;
}

void WorldDrawer::sortCommands(std::vector<Drawer::SpriteCommand>& commands) const
{
    using Command = Drawer::SpriteCommand;
    std::stable_sort(commands.begin(), commands.end(), [] (const Command& a, const Command& b)
    {
        if (a.layer != b.layer) return a.layer < b.layer;
        if (a.zIndex != b.zIndex) return a.zIndex < b.zIndex;
        return a.order < b.order;
    });
}

void WorldDrawer::submitCommands(const std::vector<Drawer::SpriteCommand>& commands) const
{
    for (const auto& cmd : commands)
    {
        Renderer::DrawTextureParams params;
        params.dest = cmd.dest;
        params.rotation = cmd.rotation;
        params.pivotX = 0.5f;
        params.pivotY = 0.5f;
        params.flipX = cmd.flipX;
        params.flipY = cmd.flipY;
        params.source = cmd.source;
        params.useSourceRect = cmd.useSourceRect;
        this->renderer.drawTexture(*cmd.texture, params);
    }
}

void WorldDrawer::updateViewports()
{
    const float scaleX = static_cast<float>(this->windowWidth) / WorldDrawer::VIRTUAL_WIDTH;
    const float scaleY = static_cast<float>(this->windowHeight) / WorldDrawer::VIRTUAL_HEIGHT;
    const float scale = std::min(scaleX, scaleY);

    const int viewW = static_cast<int>(WorldDrawer::VIRTUAL_WIDTH  * scale);
    const int viewH = static_cast<int>(WorldDrawer::VIRTUAL_HEIGHT * scale);

    const int offsetX = (this->windowWidth - viewW) / 2;
    const int offsetY = (this->windowHeight - viewH) / 2;

    this->worldViewport = { offsetX, offsetY, viewW, viewH };
}
