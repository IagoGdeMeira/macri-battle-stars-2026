#include "UILoader/UILoader.h"

#include "IUIWidgetLoader/IUIWidgetLoader.h"
#include "UIActionFactory/UIActionFactory.h"
#include "UIFactory/UIFactory.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/include/World/World.h"

#include "engine/utils/DataUtils/DataUtils.h"

void UILoader::loadStyleSheet(const std::string& path)
{
    auto root = this->parser.parse(path);
    for (auto& styleNode : root->getArray("styles")) this->styles[styleNode->getString("id")] = styleNode->clone();
}

std::vector<Entity> UILoader::loadLayout(const std::string& path)
{
    auto root = this->parser.parse(path);
    std::vector<Entity> entities;

    for (auto& elementNode : root->getArray("elements")) entities.push_back(this->createElement(*elementNode));
    return entities;
}

Entity UILoader::loadWidget(const std::string& path)
{
    auto root = this->parser.parse(path);
    if (root->isArray())
    {
        auto arr = root->getArray("");
        if (arr.empty()) return Entity{0};
        return this->createElement(*arr[0]);
    }
    return this->createElement(*root);
}

Entity UILoader::instantiateWidget(const std::string& widgetPath, const ParamMap& params, std::optional<Entity> parent)
{
    auto root = this->parser.parse(widgetPath);
    if (root->isArray())
    {
        auto arr = root->getArray("");
        if (arr.empty()) return Entity{0};
        return this->createElement(*arr[0], parent, params);
    }
    return this->createElement(*root, parent, params);
}

void UILoader::registerWidgetLoader(const std::string& widgetType, std::unique_ptr<IUIWidgetLoader> loader)
{ this->widgetLoaders[widgetType] = std::move(loader); }

Entity UILoader::createElement(const DataNode& node, std::optional<Entity> parent, const ParamMap& params)
{
    auto& comp = this->factory.world().components();

    Rectangle rect = DataUtils::parseRect(node, {{0.f, 0.f}, {100.f, 100.f}});

    std::string type = node.getString("type", "panel");
    Entity entity{0};

    if (type == "panel") entity = this->factory.createPanel(rect);
    else if (type == "text")
    {
        const std::string text = node.getString("text", "");
        float fontSize = node.getFloat("fontSize", 24.f);
        Color color = Color::WHITE();
        if (node.has("color"))
        {
            auto colorNode = node.getObject("color");
            if (colorNode) color = DataUtils::parseColor(*colorNode, Color::WHITE());
        }
        entity = this->factory.createText(text, fontSize, color, rect.position);
    }
    else if (type == "image") entity = this->factory.createImage(node.getString("image", ""), rect);
    else if (type == "button")
    {
        const std::string text = node.getString("text", "");
        std::shared_ptr<IUIAction> action = nullptr;
        if (node.has("action")) action = std::shared_ptr<IUIAction>(this->actionFactory.create(node.getString("action")));
        entity = this->factory.createButton(text, rect, action);
    }
    else if (type == "widget")
    {
        if (node.has("source"))
        {
            auto sourcePath = node.getString("source");
            auto externalNode = this->parser.parse(sourcePath);
            
            if (externalNode->isArray())
            {
                auto arr = externalNode->getArray("");
                if (arr.empty()) return Entity{0};
                entity = this->createElement(*arr[0], parent, params);
            }
            else entity = this->createElement(*externalNode, parent, params);

            return entity;
        }
        else entity = this->createWidget(node, params);
    }

    if (node.has("style"))
    {
        auto it = this->styles.find(node.getString("style"));
        if (it != this->styles.end()) this->applyStyle({entity, *it->second});
    }

    this->applyFlex({entity, node});
    this->applyBoxModel({entity, node});
    this->applyLayoutDirty({entity, node});
    this->applyAction({entity, node});

    if (parent.has_value()) comp.add<ParentComponent>(entity, ParentComponent{*parent});
    this->applyChildren({entity, node}, params);

    return entity;
}

Entity UILoader::createWidget(const DataNode& node, const ParamMap& params)
{
    std::string widgetType = node.getString("widgetType", "");
    auto it = this->widgetLoaders.find(widgetType);
    if (it == this->widgetLoaders.end()) throw std::runtime_error("No widget loader registered for type: " + widgetType);

    return it->second->load(node, params);
}

void UILoader::applyStyle(const ApplyParams& params)
{
    this->applyFlex(params);
    this->applyBoxModel(params);
    this->applyLayoutDirty(params);
}

void UILoader::applyFlex(const ApplyParams& params)
{
    if (!params.node.has("flex")) return;

    auto flexNode = params.node.getObject("flex");
    if (!flexNode) return;

    using Direction = FlexContainer::FlexDirection;
    using Justify = FlexContainer::JustifyContent;
    using Align = FlexContainer::AlignItems;

    FlexContainer flex;
    flex.direction = static_cast<Direction>(flexNode->getInt("direction", static_cast<int>(Direction::Row)));
    flex.justify = static_cast<Justify>(flexNode->getInt("justify", static_cast<int>(Justify::FlexStart)));
    flex.align = static_cast<Align>(flexNode->getInt("align", static_cast<int>(Align::Stretch)));
    flex.gap = flexNode->getFloat("gap", 0.f);

    auto& comp = this->factory.world().components();
    if (comp.has<FlexContainer>(params.entity)) comp.get<FlexContainer>(params.entity) = flex;
    else comp.add<FlexContainer>(params.entity, flex);
}

void UILoader::applyBoxModel(const ApplyParams& params)
{
    if (!params.node.has("boxModel")) return;

    auto boxNode = params.node.getObject("boxModel");
    if (!boxNode) return;

    BoxModel box;

    if (boxNode->has("margin"))
    {
        auto marginNode = boxNode->getObject("margin");
        if (marginNode) box.margin = DataUtils::parseAABB(*marginNode, {0, 0, 0, 0});
    }
    if (boxNode->has("padding"))
    {
        auto paddingNode = boxNode->getObject("padding");
        if (paddingNode) box.padding = DataUtils::parseAABB(*paddingNode, {0, 0, 0, 0});
    }
    if (boxNode->has("border"))
    {
        auto borderNode = boxNode->getObject("border");
        if (borderNode) box.border = DataUtils::parseAABB(*borderNode, {0, 0, 0, 0});
    }
    if (boxNode->has("borderRadius"))
    {
        auto radiusNode = boxNode->getObject("borderRadius");
        if (radiusNode) box.borderRadius = DataUtils::parseCorners(*radiusNode, {0, 0, 0, 0});
    }

    auto& comp = this->factory.world().components();
    if (comp.has<BoxModel>(params.entity)) comp.get<BoxModel>(params.entity) = box;
    else comp.add<BoxModel>(params.entity, box);
}

void UILoader::applyLayoutDirty(const ApplyParams& params)
{
    bool dirty = params.node.getBool("dirty", true);

    auto& comp = this->factory.world().components();
    if (comp.has<LayoutDirtyComponent>(params.entity)) comp.get<LayoutDirtyComponent>(params.entity).dirty = dirty;
    else comp.add<LayoutDirtyComponent>(params.entity, LayoutDirtyComponent{dirty});
}

void UILoader::applyAction(const ApplyParams& params)
{
    if (!params.node.has("action")) return;

    auto action = std::shared_ptr<IUIAction>(this->actionFactory.create(params.node.getString("action")));

    auto& comp = this->factory.world().components();
    auto& entity = params.entity;
    if (comp.has<UIActionComponent>(entity)) comp.get<UIActionComponent>(entity).execute = [action]() { action->execute(); };
    else comp.add<UIActionComponent>(entity, UIActionComponent{[action]() { action->execute(); }});
}

void UILoader::applyChildren(const ApplyParams& params, const ParamMap& paramsMap)
{
    if (!params.node.has("children")) return;
    for (auto& childNode : params.node.getArray("children")) this->createElement(*childNode, params.entity, paramsMap);
}
