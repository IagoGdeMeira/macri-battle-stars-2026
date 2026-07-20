#ifndef menu_loader_h
#define menu_loader_h

#include "MenuData.h"
#include "UIActionFactory/UIActionFactory.h"
#include "UIElement/UIElement.h"
#include "UIElement/TextElement.h"

#include "engine/include/DataParser/DataParser.h"

#include <memory>

class MenuLoader
{
public:
    MenuLoader(DataParser& parser, UIActionFactory& actionFactory) :
        parser(parser), actionFactory(actionFactory) {}

    MenuData load(const std::string& path) const;

private:
    DataParser& parser;
    UIActionFactory& actionFactory;

    std::unique_ptr<UIElement> parseElement(const DataNode& node) const;
    void parseBaseFields(const DataNode& node, UIElement& elem) const;
    void parseTextFields(const DataNode& node, TextElement& elem) const;
};

#endif // menu_loader_h
