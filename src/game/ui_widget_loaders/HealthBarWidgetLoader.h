#ifndef health_bar_widget_loader_h
#define health_bar_widget_loader_h

#include "IUIWidgetLoader/IParametrizedUIWidgetLoader.h"
#include "UILayerBuilder/UILayerBuilder.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/DataParser/DataParser.h"

#include <string>
#include <vector>

class UIFactory;

class HealthBarWidgetLoader : public IParametrizedUIWidgetLoader
{
public:
    explicit HealthBarWidgetLoader(UIFactory& factory, DataParser& parser) : factory(factory), parser(parser), layerBuilder(factory) {}

    Entity load(const DataNode& node, const ParamMap& params) override;

private:
    struct BarData
    {
        struct Paths { std::string characterName, font, avatar; };

        uint32_t playerId;
        int maxHealth, currentHealth;
        Dimension2D fullSize, barSize;
        Position barOffset, avatarOffset, nameOffset;
        float avatarSize, fontSize, photoScale;
        int photoZIndex;
        Paths paths;
        bool flip;

        bool hasEndCap = false;
        float capWidth = 10.f;
        Polygon capPolygon;
    };

    struct Context { Entity container; const BarData& data; };
    struct SegmentColor { Color fill = Color::WHITE(), shadow = Color::WHITE(); };

    UIFactory& factory;
    DataParser& parser;
    UILayerBuilder layerBuilder;

    static constexpr float SHADOW_HEIGHT_RATIO = 0.3f;
    static constexpr const char* DEFAULT_COLORS_PATH = "assets/ui/health_bar_colors.json";

    std::vector<SegmentColor> segmentColors;

    Entity createContainer(const BarData& data) const;
    void createSegments(const Context& ctx) const;
    void createNameLabel(const Context& ctx) const;
    void createEndCap(const Context& ctx) const;

    void loadSegmentColors();
    SegmentColor getSegmentColor(int index) const;
};

#endif // health_bar_widget_loader_h
