#include "UIFactory/UIFactory.h"

#include "IUIAction/IUIAction.h"
#include "UIElement/TextElement.h"
#include "UIElement/UIElement.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"
#include "domain/resources/Font/Font.h"
#include "domain/resources/Texture/Texture.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/include/IPlatformFactory/IPlatformFactory.h"

#include <stdexcept>

Entity UIFactory::createPanel(const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UIRectComponent>(e, UIRectComponent{rect.size});
    comp.add<FlexContainer>(e, FlexContainer{});
    comp.add<LayoutDirtyComponent>(e, LayoutDirtyComponent{true});
    this->applyDefaultBoxModel(e);
    return e;
}

Entity UIFactory::createButton(const std::string& text, const Rectangle& rect, std::shared_ptr<IUIAction> action)
{
    Entity button = this->createPanel(rect);
    auto& comp = this->factoryWorld.components();

    comp.add<UIFocusable>(button, UIFocusable{ true });

    auto& flex = comp.get<FlexContainer>(button);
    flex.direction = FlexContainer::FlexDirection::Row;
    flex.justify = FlexContainer::JustifyContent::Center;
    flex.align = FlexContainer::AlignItems::Center;

    Entity textEntity = this->createText(text, 24.f, Color::WHITE(), {0, 0});
    comp.add<ParentComponent>(textEntity, ParentComponent{ button });
    comp.get<FlexItem>(textEntity);

    if (action) comp.add<UIActionComponent>(button, UIActionComponent{[action]() { action->execute(); }});
    return button;
}

Entity UIFactory::createText(const std::string& text, float fontSize, const Color& color, const Position& position)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{position, {1.f, 1.f}, 0.f});
    comp.add<UIRectComponent>(e, UIRectComponent{{0.f, 0.f}});
    comp.add<FlexItem>(e, FlexItem{});

    std::shared_ptr<Font> font = nullptr;
    try { font = this->fontFactory.createFont("assets/fonts/default.ttf"); }
    catch (const std::exception&) { LOG_ERROR("Failed to create font for text: " + text); }

    comp.add<UITextComponent>(e, UITextComponent{ font, text, color, true, fontSize });
    return e;
}

Entity UIFactory::createImage(const std::string& texturePath, const Rectangle& rect)
{
    Entity e = this->factoryWorld.entities().create();
    auto& comp = this->factoryWorld.components();

    comp.add<TransformComponent>(e, TransformComponent{rect.position, {1.f, 1.f}, 0.f});
    comp.add<UIRectComponent>(e, UIRectComponent{rect.size});

    auto texture = this->textureFactory.createTexture(texturePath);
    comp.add<UISpriteComponent>(e, UISpriteComponent{ texture, Color::WHITE() });

    return e;
}

Entity UIFactory::createFromElement(const UIElement& element)
{
    if (auto* panel = dynamic_cast<const PanelElement*>(&element)) return this->createPanel(panel->rect);
    if (auto* text = dynamic_cast<const TextElement*>(&element))
    { return this->createText(text->text, text->fontSize, text->color, text->rect.position); }
    if (auto* button = dynamic_cast<const ButtonElement*>(&element))
    { return this->createButton(button->text, button->rect, button->action); }
    if (auto* img = dynamic_cast<const ImageElement*>(&element))
    { return this->createImage(img->imagePath, img->rect); }

    throw std::runtime_error("Unsupported UIElement type: " + element.id);
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
