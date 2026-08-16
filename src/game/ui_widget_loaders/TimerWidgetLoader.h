#ifndef timer_widget_loader_h
#define timer_widget_loader_h

#include "IUIWidgetLoader/IUIWidgetLoader.h"

class TimerWidgetLoader : public IUIWidgetLoader
{
public:
    explicit TimerWidgetLoader(UIFactory& factory) : factory(factory) {}
    Entity load(const DataNode& node, const UILoader::ParamMap& params) override;

private:
    UIFactory& factory;
};

#endif // timer_widget_loader_h
