#include "MenuLoader/MenuLoader.h"

#include "DataUtils/DataUtils.h"
#include "UIElement/TextElement.h"
#include "UIElement/UIElement.h"

#include <stdexcept>

MenuData MenuLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);
    MenuData data;
    data.id = root->getString("id");

    for (auto& elemNode : root->getArray("elements")) data.elements.push_back(this->parseElement(*elemNode));

    return data;
}

std::unique_ptr<UIElement> MenuLoader::parseElement(const DataNode& node) const
{
    std::string typeStr = node.getString("type");

    if (typeStr == "Panel")
    {
        auto elem = std::make_unique<PanelElement>();
        this->parseBaseFields(node, *elem);
        elem->bgColor = node.has("bgColor") ? DataUtils::parseColor(*node.getObject("bgColor")) : Color();
        return elem;
    }
    else if (typeStr == "Text")
    {
        auto elem = std::make_unique<TextElement>();
        this->parseBaseFields(node, *elem);
        this->parseTextFields(node, *elem);
        return elem;
    }
    else if (typeStr == "Button")
    {
        auto elem = std::make_unique<ButtonElement>();
        this->parseBaseFields(node, *elem);
        this->parseTextFields(node, *elem);
        elem->fontPath = node.getString("font", "assets/fonts/default.ttf");

        if (node.has("action"))
        {
            auto actionNode = node.getObject("action");
            std::string actionType = actionNode->getString("type");
            elem->action = this->actionFactory.create(actionType);
        }
        return elem;
    }
    else if (typeStr == "Image")
    {
        auto elem = std::make_unique<ImageElement>();
        this->parseBaseFields(node, *elem);
        elem->imagePath = node.getString("texture");
        return elem;
    }

    throw std::runtime_error("Unknown menu element type: " + typeStr);
}

void MenuLoader::parseBaseFields(const DataNode& node, UIElement& elem) const
{
    elem.id = node.getString("id", "");

    elem.rect = DataUtils::parseRect(*node.getObject("rect"), Rectangle());

    auto& model = elem.boxModel;
    model.margin = DataUtils::parseAABB(*node.getObject("margin"), AABB());
    model.padding = DataUtils::parseAABB(*node.getObject("padding"), AABB());
    model.border = DataUtils::parseAABB(*node.getObject("border"), AABB());
    model.borderRadius = DataUtils::parseCorners(*node.getObject("borderRadius"), Corners());
}

void MenuLoader::parseTextFields(const DataNode& node, TextElement& elem) const
{
    elem.text = node.getString("text");
    elem.fontSize = node.getFloat("fontSize", 24.f);
    elem.color = node.has("color") ? DataUtils::parseColor(*node.getObject("color")) : Color();
    elem.fontPath = node.getString("font", "assets/fonts/default.ttf");
}

