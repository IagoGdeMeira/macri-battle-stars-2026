#ifndef i_ui_widget_loader_h
#define i_ui_widget_loader_h

#include "UILoader/UILoader.h"

#include "domain/include/Entity/Entity.h"

#include "engine/include/DataNode/DataNode.h"

#include <unordered_map>

class IUIWidgetLoader
{
public:
    virtual ~IUIWidgetLoader() = default;
    virtual Entity load(const DataNode& node, const UILoader::ParamMap& params) = 0;
};

#endif // i_ui_widget_loader_h
