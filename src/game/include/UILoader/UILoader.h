#ifndef ui_loader_h
#define ui_loader_h

#include "UIStyleLoader/UIStyleLoader.h"

#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class DataNode;
class IFontFactory;
class IUIWidgetLoader;
class UIActionFactory;
class UIFactory;

class UILoader
{
public:
    using ParamMap = std::unordered_map<std::string, std::string>;

    UILoader(DataParser& parser, UIFactory& factory, UIActionFactory& actionFactory, IFontFactory& fontFactory) :
        parser(parser), factory(factory), actionFactory(actionFactory), fontFactory(fontFactory), styleLoader(parser) {}

    void loadStyleSheet(const std::string& path);
    std::vector<Entity> loadLayout(const std::string& path);
    Entity loadWidget(const std::string& path);

    Entity instantiateWidget(const std::string& widgetPath, const ParamMap& params, std::optional<Entity> parent = std::nullopt);
    void registerWidgetLoader(const std::string& widgetType, std::unique_ptr<IUIWidgetLoader> loader);

private:
    DataParser& parser;
    UIFactory& factory;
    UIActionFactory& actionFactory;
    IFontFactory& fontFactory;

    UIStyleLoader styleLoader;
    UIStyleLoader::StyleMap styles;

    std::unordered_map<std::string, std::unique_ptr<IUIWidgetLoader>> widgetLoaders;

    Entity createElement(const DataNode& node, std::optional<Entity> parent = std::nullopt, const ParamMap& params = {});
    Entity createWidget(const DataNode& node, const ParamMap& params);

    struct ApplyParams { Entity entity; const DataNode& node; };
    void applyStyle(const ApplyParams& params);
    void applyFlex(const ApplyParams& params);
    void applyFlexItem(const ApplyParams& params);
    void applyBoxModel(const ApplyParams& params);
    void applyLayoutDirty(const ApplyParams& params);
    void applyAction(const ApplyParams& params);
    void applyChildren(const ApplyParams& params, const ParamMap& paramsMap);
    void markParentDirty(Entity parent);
};

#endif // ui_loader_h
