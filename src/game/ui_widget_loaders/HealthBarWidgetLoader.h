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

    struct BarData { uint32_t playerId; int maxHealth, currentHealth; float width, height; };
    struct SegmentData { float hp, maxHP, maxWidth; int index; };

    Entity createContainer(const BarData& data) const;
    void createBackground(Entity container, const BarData& data) const;
    void createBorder(Entity container, const BarData& data) const;
    void createSegments(Entity container, const BarData& data) const;

    struct SegmentParams { float remainingHP = 0.f, maxHP = 100.f, maxWidth = 0.f; };
    SegmentData computeSegment(int index, const SegmentParams& params) const;
};

#endif // health_bar_widget_loader_h
