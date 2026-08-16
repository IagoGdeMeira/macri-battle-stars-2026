#ifndef ui_loader_h
#define ui_loader_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class UIFactory;
class UIActionFactory;
class DataNode;
class IUIWidgetLoader;

class UILoader
{
public:
    using ParamMap = std::unordered_map<std::string, std::string>;

    UILoader(DataParser& parser, UIFactory& factory, UIActionFactory& actionFactory) :
        parser(parser), factory(factory), actionFactory(actionFactory) {}

    void loadStyleSheet(const std::string& path);
    std::vector<Entity> loadLayout(const std::string& path);
    Entity loadWidget(const std::string& path);
    
    Entity instantiateWidget(const std::string& widgetPath, const ParamMap& params, std::optional<Entity> parent = std::nullopt);
    void registerWidgetLoader(const std::string& widgetType, std::unique_ptr<IUIWidgetLoader> loader);

private:
    DataParser& parser;
    UIFactory& factory;
    UIActionFactory& actionFactory;

    using StyleMap = std::unordered_map<std::string, std::unique_ptr<DataNode>>;
    StyleMap styles;

    std::unordered_map<std::string, std::unique_ptr<IUIWidgetLoader>> widgetLoaders;

    Entity createElement(const DataNode& node, std::optional<Entity> parent = std::nullopt, const ParamMap& params = {});
    Entity createWidget(const DataNode& node, const ParamMap& params);

    struct ApplyParams { Entity entity; const DataNode& node; };
    void applyStyle(const ApplyParams& params);
    void applyFlex(const ApplyParams& params);
    void applyBoxModel(const ApplyParams& params);
    void applyLayoutDirty(const ApplyParams& params);
    void applyAction(const ApplyParams& params);
    void applyChildren(const ApplyParams& params, const ParamMap& paramsMap);
};

#endif // ui_loader_h
