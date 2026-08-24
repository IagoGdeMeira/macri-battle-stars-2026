#ifndef i_parametrized_ui_widget_loader_h
#define i_parametrized_ui_widget_loader_h

#include "IUIWidgetLoader.h"

#include <string>
#include <unordered_map>

class IParametrizedUIWidgetLoader : public IUIWidgetLoader
{
public:
    using ParamMap = std::unordered_map<std::string, std::string>;

    Entity load(const DataNode& node) override { return this->load(node, {}); }
    virtual Entity load(const DataNode& node, const ParamMap& params) = 0;
};

#endif // i_parametrized_ui_widget_loader_h
