#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarEndCapComponent.h"
#include "domain/components/HealthBarLayoutComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/PolygonShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"
#include "domain/resources/Font/Font.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/utils/DataUtils/DataUtils.h"

#include <algorithm>

Entity HealthBarWidgetLoader::load(const DataNode& node, const ParamMap& params)
{
    this->loadSegmentColors();

    BarData data;
    data.playerId               = static_cast<uint32_t>(std::stoul(params.at("playerId")));
    data.maxHealth              = std::stoi(params.at("maxHealth"));
    data.currentHealth          = std::stoi(params.at("currentHealth"));
    data.fontSize               = node.getFloat("fontSize", 16.f);
    data.avatarSize             = node.getFloat("avatarSize", 50.f);
    data.paths.characterName    = params.count("characterName") ? params.at("characterName") : "";
    data.paths.font             = node.getString("fontPath", "assets/fonts/default.ttf");
    data.paths.avatar           = params.count("avatarTexturePath") ? params.at("avatarTexturePath") : "";
    data.flip                   = (node.getString("direction", "left") == "right");
    data.photoScale             = 0.8f;
    data.photoZIndex            = 60;

    if (node.has("size")) data.fullSize = DataUtils::parseSize(node);
    else data.fullSize = Dimension2D{node.getFloat("width", 300.f), node.getFloat("height", 30.f)};

    float avatarSpace = (data.avatarSize > 0.f) ? data.avatarSize + 10.f : 0.f;
    data.barSize.height = node.getFloat("barHeight", data.fullSize.height * 0.6f);
    data.barSize.width  = data.fullSize.width - avatarSpace;

    if (data.flip)
    {
        data.avatarOffset = Position{data.fullSize.width - data.avatarSize, (data.fullSize.height - data.avatarSize) * 0.5f};
        data.barOffset    = Position{0.f, (data.fullSize.height - data.barSize.height) * 0.5f};
    }
    else
    {
        data.avatarOffset = Position{0.f, (data.fullSize.height - data.avatarSize) * 0.5f};
        data.barOffset    = Position{avatarSpace, (data.fullSize.height - data.barSize.height) * 0.5f};
    }

    data.nameOffset = Position{data.barOffset.x, data.fullSize.height - data.fontSize * 0.8f};

    if (node.has("avatar"))
    {
        auto avatarNode = node.getObject("avatar");
        if (avatarNode && avatarNode->has("photo"))
        {
            auto photoNode = avatarNode->getObject("photo");
            if (photoNode)
            {
                data.photoScale  = photoNode->getFloat("scale", 0.8f);
                data.photoZIndex = photoNode->getInt("zIndex", 60);
            }
        }
    }

    if (node.has("endCap"))
    {
        auto capNode = node.getObject("endCap");
        if (capNode)
        {
            data.hasEndCap = true;
            data.capWidth = capNode->getFloat("width", 10.f);

            if (capNode->has("vertices")) for (auto& vNode : capNode->getArray("vertices"))
            {
                Position p;
                p.x = vNode->getFloat("x", 0.f);
                p.y = vNode->getFloat("y", 0.f);
                data.capPolygon.vertices.push_back(p);
            }
            else data.capPolygon.vertices = {Position{0.f, 0.f}, Position{data.capWidth, 0.f}, Position{0.f, data.barSize.height}};

            if (data.flip) for (auto& v : data.capPolygon.vertices) v.x = data.capWidth - v.x;
        }
    }

    Entity container = this->createContainer(data);
    Context ctx{container, data};

    if (node.has("bar"))
    {
        auto barNode = node.getObject("bar");
        if (barNode) this->layerBuilder.build(container, *barNode, data.barOffset, data.barSize);
    }

    this->createSegments(ctx);

    if (node.has("avatar"))
    {
        auto avatarNode = node.getObject("avatar");
        if (avatarNode)
        {
            Dimension2D avatarSize{data.avatarSize, data.avatarSize};
            this->layerBuilder.build(container, *avatarNode, data.avatarOffset, avatarSize);
        }
    }
    if (!data.paths.avatar.empty())
    {
        float innerSize = data.avatarSize * data.photoScale;
        Position innerOffset = {
            data.avatarOffset.x + (data.avatarSize - innerSize) * 0.5f,
            data.avatarOffset.y + (data.avatarSize - innerSize) * 0.5f
        };
        Entity photo = this->factory.createImage(data.paths.avatar,
            Rectangle{Position{0.f, 0.f}, Dimension2D{innerSize, innerSize}});
        this->factory.attachChild(container, photo, innerOffset);
        this->factory.world().components().get<RenderComponent>(photo).zIndex = data.photoZIndex;
    }

    this->createNameLabel(ctx);
    this->createEndCap(ctx);

    return container;
}

Entity HealthBarWidgetLoader::createContainer(const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity container = this->factory.createBox(Rectangle{Position{0.f, 0.f}, data.fullSize});
    comp.add<HealthBarTag>(container, HealthBarTag{data.playerId, data.maxHealth, data.currentHealth});
    return container;
}

void HealthBarWidgetLoader::createSegments(const Context& ctx) const
{
    const BarData& data = ctx.data;
    const float maxSegmentHP = 100.f;
    const float barTotalWidth = data.barSize.width;
    const float shadowHeight = data.barSize.height * HealthBarWidgetLoader::SHADOW_HEIGHT_RATIO;
    const float fillHeight = data.barSize.height - shadowHeight;

    for (int i = 0; ; ++i)
    {
        float rangeStartHP = i * maxSegmentHP;
        if (rangeStartHP >= data.maxHealth) break;

        float rangeEndHP = std::min(rangeStartHP + maxSegmentHP, static_cast<float>(data.maxHealth));
        float rangeMaxHP = rangeEndHP - rangeStartHP;
        float hpInRange = std::max(0.f, std::min(static_cast<float>(data.currentHealth) - rangeStartHP, rangeMaxHP));
        float fillFraction = (rangeMaxHP > 0.f) ? hpInRange / rangeMaxHP : 0.f;
        float segmentWidth = barTotalWidth * fillFraction;

        SegmentColor colors = this->getSegmentColor(i);

        float posX = data.barOffset.x;
        if (data.flip) posX += barTotalWidth - segmentWidth;

        Entity fillSegment = this->factory.createRectangleShape(
            UIFactory::ShapeParams{colors.fill},
            Rectangle{Position{0.f, 0.f}, Dimension2D{segmentWidth, fillHeight}});
        this->factory.attachChild(ctx.container, fillSegment, {posX, data.barOffset.y});

        Entity shadowSegment = this->factory.createRectangleShape(
            UIFactory::ShapeParams{colors.shadow},
            Rectangle{Position{0.f, 0.f}, Dimension2D{segmentWidth, shadowHeight}});
        this->factory.attachChild(ctx.container, shadowSegment, {posX, data.barOffset.y + fillHeight});

        auto& comp = this->factory.world().components();

        comp.add<HealthBarSegmentComponent>(fillSegment, HealthBarSegmentComponent{
            rangeMaxHP, barTotalWidth, colors.fill, colors.shadow, i});
        comp.add<HealthBarSegmentComponent>(shadowSegment, HealthBarSegmentComponent{
            rangeMaxHP, barTotalWidth, colors.shadow, colors.shadow, i});

        comp.get<RenderComponent>(fillSegment).zIndex = 1 + i;
        comp.get<RenderComponent>(shadowSegment).zIndex = 1 + i;
    }
}

void HealthBarWidgetLoader::createNameLabel(const Context& ctx) const
{
    if (ctx.data.paths.characterName.empty()) return;

    auto& comp = this->factory.world().components();
    std::shared_ptr<Font> font = nullptr;
    try { font = this->factory.fontFactory().createFont(ctx.data.paths.font); }
    catch (const std::exception& e) { LOG_ERROR("Failed to load font for name: {}", e.what()); }

    UIFactory::TextParams textParams{
        .text       = ctx.data.paths.characterName,
        .fontSize   = ctx.data.fontSize,
        .color      = Color::WHITE(),
        .position   = ctx.data.nameOffset
    };

    Entity label = this->factory.createText(textParams, font);
    this->factory.attachChild(ctx.container, label, ctx.data.nameOffset);
    comp.get<RenderComponent>(label).zIndex = 80;
}

void HealthBarWidgetLoader::createEndCap(const Context& ctx) const
{
    if (!ctx.data.hasEndCap || ctx.data.capPolygon.vertices.size() < 3) return;

    auto& comp = this->factory.world().components();

    HealthBarLayoutComponent layout;
    layout.flip = ctx.data.flip;
    layout.barOffsetX = ctx.data.barOffset.x;
    layout.barHeight = ctx.data.barSize.height;
    layout.capWidth = ctx.data.capWidth;
    comp.add<HealthBarLayoutComponent>(ctx.container, layout);

    Entity cap = this->factory.createPolygonShape(
        UIFactory::ShapeParams{Color::WHITE(), true},
        ctx.data.capPolygon);
    comp.get<PolygonShapeComponent>(cap).zIndex = 500;
    this->factory.attachChild(ctx.container, cap, Position{ctx.data.barOffset.x, ctx.data.barOffset.y});
    comp.add<HealthBarEndCapComponent>(cap, HealthBarEndCapComponent{});
}

void HealthBarWidgetLoader::loadSegmentColors()
{
    this->segmentColors.clear();

    try
    {
        auto root = this->parser.parse(HealthBarWidgetLoader::DEFAULT_COLORS_PATH);
        if (!root) return;

        auto segmentsNode = root->getArray("segments");
        for (auto& segNode : segmentsNode)
        {
            if (!segNode) continue;

            SegmentColor color;
            auto fillNode = segNode->getObject("fill");
            if (fillNode) color.fill = DataUtils::parseColor(*fillNode, Color::WHITE());

            auto shadowNode = segNode->getObject("shadow");
            if (shadowNode) color.shadow = DataUtils::parseColor(*shadowNode, Color::WHITE());

            this->segmentColors.push_back(color);
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed to load health bar colors from JSON: {}", e.what());
        this->segmentColors = {
            { Color{255, 0, 0, 255},     Color{120, 0, 0, 255}     },
            { Color{255, 255, 0, 255},   Color{120, 120, 0, 255}   },
            { Color{0, 255, 0, 255},     Color{0, 120, 0, 255}     },
            { Color{0, 255, 255, 255},   Color{0, 120, 120, 255}   },
            { Color{0, 0, 255, 255},     Color{0, 0, 120, 255}     },
            { Color{128, 0, 128, 255},   Color{64, 0, 64, 255}     },
            { Color{200, 100, 255, 255}, Color{100, 50, 128, 255}  },
            { Color{200, 200, 200, 255}, Color{100, 100, 100, 255} }
        };
    }
}

HealthBarWidgetLoader::SegmentColor HealthBarWidgetLoader::getSegmentColor(int index) const
{
    if (index >= 0 && index < static_cast<int>(this->segmentColors.size())) return this->segmentColors[index];
    return this->segmentColors.empty() ? SegmentColor{} : this->segmentColors.back();
}
