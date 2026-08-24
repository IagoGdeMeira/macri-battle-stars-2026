#ifndef i_ui_widget_loader_h
#define i_ui_widget_loader_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/DataNode/DataNode.h"

class IUIWidgetLoader
{
public:
    virtual ~IUIWidgetLoader() = default;
    virtual Entity load(const DataNode& node) = 0;
};

#endif // i_ui_widget_loader_h
