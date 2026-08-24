#ifndef timer_widget_loader_h
#define timer_widget_loader_h

#include "IUIWidgetLoader/IParametrizedUIWidgetLoader.h"

class IFontFactory;
class UIFactory;

class TimerWidgetLoader : public IParametrizedUIWidgetLoader
{
public:
    TimerWidgetLoader(UIFactory& factory, IFontFactory& fontFactory);
    Entity load(const DataNode& node, const ParamMap& params) override;

private:
    UIFactory& factory;
    IFontFactory& fontFactory;
};

#endif // timer_widget_loader_h
