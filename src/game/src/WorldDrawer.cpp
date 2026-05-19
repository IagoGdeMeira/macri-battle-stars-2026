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
        this->windowSize = { static_cast<float>(e.width), static_cast<float>(e.height) };
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
    this->spriteBatch.clear();

    auto view = View<SpriteComponent, TransformComponent, RenderComponent>(ctx.world.components());    
    size_t order = 0;
    for (auto [entity, sprite, _t, _r] : view)
    {
        if (!sprite.texture) continue;
        this->spriteBatch.add(this->buildTextureCommand(entity, ctx.world, order++));
    }
    this->spriteBatch.submit(this->renderer);
}

void WorldDrawer::renderShapes(RenderContext& ctx)
{
    this->circleBatch.clear();
    this->rectangleBatch.clear();

    auto view = View<ShapeRenderComponent, TransformComponent>(ctx.world.components());
    size_t order = 0;
    for (auto [entity, shapeComp, _t] : view)
    {
        if (!shapeComp.shape) continue;
        switch (shapeComp.shape->getType())
        {
            case ColliderDef::ColliderType::Rectangle:
                this->rectangleBatch.add(this->buildRectangleCommand(entity, ctx.world, order++));
                break;
            case ColliderDef::ColliderType::Circle:
                this->circleBatch.add(this->buildCircleCommand(entity, ctx.world, order++));
                break;
        }
    }

    this->rectangleBatch.submit(this->renderer);
    this->circleBatch.submit(this->renderer);
}

DrawCircleCommand WorldDrawer::buildCircleCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<TransformComponent>(entity);
    const auto& shapeComp = components.get<ShapeRenderComponent>(entity);

    auto* circ = static_cast<const CircleDef*>(shapeComp.shape.get());

    Position screenPos = this->worldToScreen({transform.x, transform.y}, this->worldViewport);
    float zoom = this->camera.getZoom();

    DrawCircleCommand cmd;
    cmd.circle.position = screenPos;
    cmd.circle.radius = circ->radius * std::max(std::abs(transform.scaleX), std::abs(transform.scaleY)) * zoom;
    cmd.color = shapeComp.color;
    cmd.filled = shapeComp.filled;
    cmd.order = order;
    return cmd;
}

DrawRectangleCommand WorldDrawer::buildRectangleCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<TransformComponent>(entity);
    const auto& shapeComp = components.get<ShapeRenderComponent>(entity);

    auto* rect = static_cast<const RectangleDef*>(shapeComp.shape.get());

    Position screenPos = this->worldToScreen({transform.x, transform.y}, this->worldViewport);
    float zoom = this->camera.getZoom();

    DrawRectangleCommand cmd;
    cmd.rect.position.x = screenPos.x - (rect->width * std::abs(transform.scaleX) * zoom) * 0.5f;
    cmd.rect.position.y = screenPos.y - (rect->height * std::abs(transform.scaleY) * zoom) * 0.5f;
    cmd.rect.size.width = rect->width * std::abs(transform.scaleX) * zoom;
    cmd.rect.size.height = rect->height * std::abs(transform.scaleY) * zoom;
    cmd.color = shapeComp.color;
    cmd.filled = shapeComp.filled;
    cmd.order = order;
    return cmd;
}

DrawTextureCommand WorldDrawer::buildTextureCommand(Entity& entity, World& world, size_t order) const
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
        static_cast<float>(sprite.size.width), static_cast<float>(sprite.size.height)
    };

    DrawTextureCommand cmd;
    cmd.texture = sprite.texture.get();
    cmd.dest.position = screenPos;
    cmd.rotation = transform.rotation;

    this->computeSpriteTransform(spriteConfig, cmd);

    if (components.has<OrientationComponent>(entity))
    {
        bool symmetric = (components.has<AnimationControllerComponent>(entity))
            ? components.get<AnimationControllerComponent>(entity).animations.symmetric : true;

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
    cmd.source.position.x = static_cast<float>(sprite.source.position.x);
    cmd.source.position.y = static_cast<float>(sprite.source.position.y);
    cmd.source.size.width = static_cast<float>(sprite.source.size.width);
    cmd.source.size.height = static_cast<float>(sprite.source.size.height);
    cmd.useSourceRect = sprite.useSourceRect;
    return cmd;
}

Position WorldDrawer::worldToScreen(Position worldPos, const Viewport& viewport, Position parallax) const
{
    const float camX = this->camera.getX();
    const float camY = this->camera.getY();
    const float zoom = this->camera.getZoom();
    
    return
    {
        (worldPos.x - camX * parallax.x) * zoom + viewport.width * 0.5f,
        (worldPos.y - camY * parallax.y) * zoom + viewport.height * 0.5f
    };
}

Position WorldDrawer::resolveParallax(World& world, Entity entity) const
{
    Position p {1.0f, 1.0f};
    auto& components = world.components();
    if (components.has<ParallaxComponent>(entity))
    {
        auto& par = components.get<ParallaxComponent>(entity);
        p = { par.factorX, par.factorY };
    }
    return p;
}

void WorldDrawer::computeSpriteTransform(const Rectangle& spriteConfig, DrawTextureCommand& cmd) const
{
    const float zoom = this->camera.getZoom();
    const int width = static_cast<int>(spriteConfig.size.width * std::abs(spriteConfig.position.x) * zoom);
    const int height = static_cast<int>(spriteConfig.size.height * std::abs(spriteConfig.position.y) * zoom);

    cmd.dest.size = { static_cast<float>(width), static_cast<float>(height) };
    cmd.flipX = spriteConfig.position.x < 0.0f;
    cmd.flipY = spriteConfig.position.y < 0.0f;
}

void WorldDrawer::updateViewports()
{
    const float scale = std::min(
        this->windowSize.width / WorldDrawer::VIRTUAL_SIZE.width,
        this->windowSize.height / WorldDrawer::VIRTUAL_SIZE.height);

    const Dimension2D scaledSize
    {
        WorldDrawer::VIRTUAL_SIZE.width * scale,
        WorldDrawer::VIRTUAL_SIZE.height * scale
    };
    
    Position offset
    {
        (this->windowSize.width - scaledSize.width) * 0.5f,
        (this->windowSize.height - scaledSize.height) * 0.5f
    };

    this->worldViewport =
    {
        static_cast<int>(offset.x), static_cast<int>(offset.y),
        static_cast<int>(scaledSize.width), static_cast<int>(scaledSize.height)
    };
}
