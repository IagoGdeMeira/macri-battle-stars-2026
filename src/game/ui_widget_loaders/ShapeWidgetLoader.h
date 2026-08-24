#ifndef shape_widget_loader_h
#define shape_widget_loader_h

#include "IUIWidgetLoader/IUIWidgetLoader.h"

class UIFactory;

class ShapeWidgetLoader : public IUIWidgetLoader
{
public:
    explicit ShapeWidgetLoader(UIFactory& factory);
    Entity load(const DataNode& node) override;

private:
    UIFactory& factory;

    void applyRenderComponent(Entity entity, int zIndex, int layer);
};

#endif // shape_widget_loader_h
