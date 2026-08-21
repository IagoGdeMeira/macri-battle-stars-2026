#include "UIFactory/UIFactory.h"

#include "IUIAction/IUIAction.h"
#include "UIElement/TextElement.h"
#include "UIElement/UIElement.h"

#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"
#include "domain/resources/Font/Font.h"
#include "domain/resources/Texture/Texture.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/include/ITextureFactory/ITextureFactory.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

Entity UIFactory::createDebugOverlay(const Rectangle& rect, const DebugConfig& debug)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});
    comp.add<RectangleShapeComponent>(e, RectangleShapeComponent{rect, debug.color, debug.filled});
    comp.add<RenderComponent>(e, RenderComponent{debug.layer, debug.zIndex});

    return e;
}

Entity UIFactory::createPanel(const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});
    comp.add<FlexContainer>(e, FlexContainer{});
    comp.add<LayoutDirtyComponent>(e, LayoutDirtyComponent{true});
    this->applyDefaultBoxModel(e);
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createButton(const std::string& text, const Rectangle& rect, std::shared_ptr<IUIAction> action)
{
    Entity button = this->createPanel(rect);
    auto& comp = this->factoryWorld.components();

    comp.add<UIFocusable>(button, UIFocusable{ true });

    auto& flex = comp.get<FlexContainer>(button);
    flex.direction = FlexDirection::Row;
    flex.justify = JustifyContent::Center;
    flex.align = AlignItems::Center;

    Entity textEntity = this->createText({text, 24.f, Color::WHITE(), {0, 0}});
    this->attachChild(button, textEntity);
    comp.get<FlexItem>(textEntity);

    if (action) comp.add<UIActionComponent>(button, UIActionComponent{[action]() { action->execute(); }});
    return button;
}

Entity UIFactory::createImage(const std::string& texturePath, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});

    auto texture = this->textureFactory.createTexture(texturePath);
    comp.add<UISpriteComponent>(e, UISpriteComponent{ texture, Color::WHITE() });
    comp.add<RenderComponent>(e, RenderComponent{0, 0});

    return e;
}

Entity UIFactory::createBox(const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createRectangleShape(const ShapeParams& params, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});
    comp.add<RectangleShapeComponent>(e, RectangleShapeComponent{rect, params.color, params.filled});
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createCircleShape(const ShapeParams& params, const Circle& circle)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{circle.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{{circle.radius * 2.f, circle.radius * 2.f}});
    comp.add<CircleShapeComponent>(e, CircleShapeComponent{circle, params.color, params.filled});
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createText(const TextParams& params)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{params.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{{0.f, 0.f}});
    comp.add<FlexItem>(e, FlexItem{});

    std::shared_ptr<Font> font = nullptr;
    try { font = this->fontFactory.createFont("assets/fonts/default.ttf"); }
    catch (const std::exception&) { LOG_ERROR("Failed to create font for text: " + params.text); }

    comp.add<UITextComponent>(e, UITextComponent{ font, params.text, params.color, true, params.fontSize });
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createText(const TextParams& params, std::shared_ptr<Font> font)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{params.position, {1.f, 1.f}, 0.f});
    comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{{0.f, 0.f}});
    comp.add<FlexItem>(e, FlexItem{});
    comp.add<UITextComponent>(e, UITextComponent{ font, params.text, params.color, true, params.fontSize });
    comp.add<RenderComponent>(e, RenderComponent{0, 0});
    return e;
}

Entity UIFactory::createFromElement(const UIElement& element)
{
    if (auto* panel = dynamic_cast<const PanelElement*>(&element)) return this->createPanel(panel->rect);

    if (auto* text = dynamic_cast<const TextElement*>(&element))
    { return this->createText({text->text, text->fontSize, text->color, text->rect.position}); }

    if (auto* button = dynamic_cast<const ButtonElement*>(&element))
    { return this->createButton(button->text, button->rect, button->action); }

    if (auto* img = dynamic_cast<const ImageElement*>(&element))
    { return this->createImage(img->imagePath, img->rect); }

    throw std::runtime_error("Unsupported UIElement type: " + element.id);
}

void UIFactory::attachChild(Entity parent, Entity child, const Position& localPos)
{
    auto& comp = this->factoryWorld.components();

    if (!comp.has<ParentComponent>(child)) comp.add<ParentComponent>(child, ParentComponent{parent});
    if (!comp.has<LocalTransform>(child)) comp.add<LocalTransform>(child, LocalTransform{localPos});
    if (!comp.has<TransformComponent>(child)) comp.add<TransformComponent>(child, TransformComponent{{0.f, 0.f}});
    if (!comp.has<ChildrenComponent>(parent)) comp.add<ChildrenComponent>(parent, ChildrenComponent{});
    comp.get<ChildrenComponent>(parent).children.push_back(child);
}

void UIFactory::applyDefaultBoxModel(Entity& entity)
{
    auto& comp = this->factoryWorld.components();
    BoxModel box;
    box.margin = {};
    box.padding = {8, 8, 8, 8};
    box.border = {};
    box.borderRadius = {};
    comp.add<BoxModel>(entity, box);
}
