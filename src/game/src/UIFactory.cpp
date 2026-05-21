#include "../include/UIFactory/UIFactory.h"

#include "../../domain/components/BoxModel.h"
#include "../../domain/components/FlexContainer.h"
#include "../../domain/components/FlexItem.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/UIActionComponent.h"
#include "../../domain/components/UIFocusable.h"
#include "../../domain/components/UISpriteComponent.h"
#include "../../domain/components/UITextComponent.h"
#include "../../domain/components/UITransform.h"
#include "../../domain/include/World/World.h"

#include "../../engine/include/Font/Font.h"
#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../engine/include/Texture/Texture.h"

#include "../../game/include/IUIAction/IUIAction.h"

Entity UIFactory::createPanel(const Rectangle& rect)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<UITransform>(e, UITransform{ rect });
    comp.add<FlexContainer>(e, FlexContainer{});
    this->applyDefaultBoxModel(e);
    return e;
}

Entity UIFactory::createButton(const std::string& text, const Rectangle& rect, std::shared_ptr<IUIAction> action)
{
    Entity button = this->createPanel(rect);
    auto& comp = this->world.components();

    comp.add<UIFocusable>(button, UIFocusable{ true });

    auto& flex = comp.get<FlexContainer>(button);
    flex.direction = FlexContainer::FlexDirection::Row;
    flex.justify = FlexContainer::JustifyContent::Center;
    flex.align = FlexContainer::AlignItems::Center;

    Entity textEntity = this->createText(text, 24.0f, Color::WHITE(), {0, 0});
    comp.add<ParentComponent>(textEntity, ParentComponent{ button });
    comp.get<FlexItem>(textEntity);

    if (action) comp.add<UIActionComponent>(button, UIActionComponent{[action]() { action->execute(); }});

    return button;
}

Entity UIFactory::createText(const std::string& text, float fontSize, const Color& color, const Position& position)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<UITransform>(e, UITransform{ Rectangle{ position, { 0, 0 } } });
    comp.add<FlexItem>(e, FlexItem{});

    auto font = this->fontFactory.createFont("assets/fonts/default.ttf");
    comp.add<UITextComponent>(e, UITextComponent{ font, text, color, true, fontSize });

    return e;
}

Entity UIFactory::createImage(const std::string& texturePath, const Rectangle& rect)
{
    Entity e = this->world.entities().create();
    auto& comp = this->world.components();

    comp.add<UITransform>(e, UITransform{ rect });

    auto texture = this->textureFactory.createTexture(texturePath);
    comp.add<UISpriteComponent>(e, UISpriteComponent{ texture, Color::WHITE() });

    return e;
}

void UIFactory::applyDefaultBoxModel(Entity& entity)
{
    auto& comp = this->world.components();
    BoxModel box;
    box.margin = {};
    box.padding = {8, 8, 8, 8};
    box.border = {};
    box.borderRadius = {};
    comp.add<BoxModel>(entity, box);
}
