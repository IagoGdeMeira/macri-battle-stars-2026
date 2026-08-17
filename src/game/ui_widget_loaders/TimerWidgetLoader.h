#ifndef timer_widget_loader_h
#define timer_widget_loader_h

#include "IUIWidgetLoader/IUIWidgetLoader.h"

class IFontFactory;
class UIFactory;

class TimerWidgetLoader : public IUIWidgetLoader
{
public:
    TimerWidgetLoader(UIFactory& factory, IFontFactory& fontFactory);
    Entity load(const DataNode& node, const UILoader::ParamMap& params) override;

private:
    UIFactory& factory;
    IFontFactory& fontFactory;
};

#endif // timer_widget_loader_h
