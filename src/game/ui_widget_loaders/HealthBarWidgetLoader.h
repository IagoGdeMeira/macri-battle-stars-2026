#ifndef health_bar_widget_loader_h
#define health_bar_widget_loader_h

#include "IUIWidgetLoader/IUIWidgetLoader.h"

class HealthBarWidgetLoader : public IUIWidgetLoader
{
public:
    explicit HealthBarWidgetLoader(UIFactory& factory) : factory(factory) {}
    Entity load(const DataNode& node, const UILoader::ParamMap& params) override;

private:
    UIFactory& factory;
};

#endif // health_bar_widget_loader_h
