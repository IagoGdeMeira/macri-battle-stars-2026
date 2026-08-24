#ifndef health_bar_widget_loader_h
#define health_bar_widget_loader_h

#include "IUIWidgetLoader/IUIWidgetLoader.h"

#include "domain/value_objects/Color/Color.h"

#include "engine/include/DataParser/DataParser.h"

#include <vector>

class HealthBarWidgetLoader : public IUIWidgetLoader
{
public:
    explicit HealthBarWidgetLoader(UIFactory& factory, DataParser& parser) : factory(factory), parser(parser) {}
    Entity load(const DataNode& node, const UILoader::ParamMap& params) override;

private:
    struct BarData { uint32_t playerId; int maxHealth, currentHealth; float width, height; };
    struct SegmentColor { Color fill = Color::WHITE(), shadow = Color::WHITE(); };

    UIFactory& factory;
    DataParser& parser;

    static constexpr float SHADOW_HEIGHT_RATIO = 0.3f;
    static constexpr const char* DEFAULT_COLORS_PATH = "assets/ui/health_bar_colors.json";

    std::vector<SegmentColor> segmentColors;

    Entity createContainer(const BarData& data) const;
    void createBackground(Entity container, const BarData& data) const;
    void createBorder(Entity container, const BarData& data) const;
    void createSegments(Entity container, const BarData& data) const;

    void loadSegmentColors();
    SegmentColor getSegmentColor(int index) const;
};

#endif // health_bar_widget_loader_h
