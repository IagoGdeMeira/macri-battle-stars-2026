#include "../include/UIDrawer/UIDrawer.h"

#include "../../domain/components/UITransform.h"
#include "../../domain/components/UISpriteComponent.h"
#include "../../domain/components/UITextComponent.h"
#include "../../domain/components/BoxModel.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/RenderContext/RenderContext.h"

void UIDrawer::draw(RenderContext& ctx)
{
    this->renderer.setViewport(uiViewport);
    this->renderSprites(ctx);
    this->renderTexts(ctx);
    this->renderShapes(ctx);
}

void UIDrawer::renderSprites(RenderContext& ctx)
{
    this->textureBatch.clear();
    auto view = View<UISpriteComponent, UITransform>(ctx.world.components());
    size_t order = 0;
    for (auto [entity, sprite, _] : view)
    {
        if (!sprite.texture) continue;
        this->textureBatch.add(this->buildTextureCommand(entity, ctx.world, order++));
    }
    this->textureBatch.submit(this->renderer);
}

void UIDrawer::renderTexts(RenderContext& ctx)
{
    this->fontBatch.clear();
    auto view = View<UITransform, UITextComponent>(ctx.world.components());
    size_t order = 0;
    for (auto [entity, transform, text] : view)
    {
        if (text.text.empty() || !text.font) continue;
        this->fontBatch.add(this->buildFontCommand(entity, ctx.world, order++));
    }
    this->fontBatch.submit(this->renderer);
}

void UIDrawer::renderShapes(RenderContext& ctx)
{
    this->rectangleBatch.clear();
    this->circleBatch.clear();
    
    // Render shapes here when needed.
    // For now, the UI only uses sprites and text, so this is left unimplemented.
    
    this->rectangleBatch.submit(this->renderer);
    this->circleBatch.submit(this->renderer);
}

DrawTextureCommand UIDrawer::buildTextureCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<UITransform>(entity);
    const auto& sprite = components.get<UISpriteComponent>(entity);

    Position pos = this->applyAnchor(transform);

    DrawTextureCommand cmd;
    cmd.texture = sprite.texture.get();
    cmd.dest.position = pos;
    cmd.dest.size = transform.rect.size;
    cmd.rotation = transform.rotation;
    cmd.flipX = (transform.scale.x < 0.f);
    cmd.flipY = (transform.scale.y < 0.f);
    cmd.layer = 0;
    cmd.zIndex = 0;
    cmd.order = order;
    cmd.tint = sprite.tint;
    cmd.blend = BlendMode::Normal;
    return cmd;
}

DrawFontCommand UIDrawer::buildFontCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<UITransform>(entity);
    const auto& text = components.get<UITextComponent>(entity);

    Position pos = this->applyAnchor(transform);

    DrawFontCommand cmd;
    cmd.text = text.text;
    cmd.font = text.font.get();
    cmd.dest.position = pos;
    cmd.dest.size = transform.rect.size;
    cmd.fontSize = static_cast<int>(text.fontSize > 0 ? text.fontSize : 16);
    cmd.color = text.color;
    cmd.layer = 0;
    cmd.zIndex = 0;
    cmd.order = order;
    return cmd;
}

DrawRectangleCommand UIDrawer::buildRectangleCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<UITransform>(entity);

    // Obtains color, filled, and other properties from a hypothetical BoxModel component in the future.
    
    DrawRectangleCommand cmd;
    cmd.rect = transform.rect;
    cmd.color  = Color::WHITE();
    cmd.filled = false;
    cmd.order = order;
    return cmd;
}

DrawCircleCommand UIDrawer::buildCircleCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& transform = components.get<UITransform>(entity);
    
    DrawCircleCommand cmd;
    cmd.circle.position =
    {
        transform.rect.position.x + transform.rect.size.width * 0.5f,
        transform.rect.position.y + transform.rect.size.height * 0.5f
    };
    cmd.circle.radius = std::min(transform.rect.size.width, transform.rect.size.height) * 0.5f;
    cmd.color = Color::WHITE();
    cmd.filled = false;
    cmd.order = order;
    return cmd;
}

Position UIDrawer::applyAnchor(const UITransform& transform) const
{
    // For now, just return the position of the rect.
    // In the future, it may calculate the offset based on the anchor and the viewport size.
    return transform.rect.position;
}
