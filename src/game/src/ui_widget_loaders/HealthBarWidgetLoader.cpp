#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"
#include "domain/resources/Font/Font.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/Color/Color.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/include/ITextureFactory/ITextureFactory.h"

#include <algorithm>
#include <cstdlib>
#include <vector>

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
    data.paths.barFrame         = node.getString("barFrameTexture", "");
    data.paths.avatarFrame      = node.getString("avatarBackgroundTexture", "");
    data.paths.avatarBorder     = node.getString("avatarBorderTexture", "");
    data.flip                   = (node.getString("direction", "left") == "right");

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
        data.barOffset    = Position{data.avatarSize + 10.f, (data.fullSize.height - data.barSize.height) * 0.5f};
    }

    data.nameOffset = Position{data.barOffset.x, data.fullSize.height - data.fontSize * 0.8f};

    Entity container = this->createContainer(data);

    this->createBackground(container, data);
    this->createSegments(container, data);
    this->createBarFrameTexture(container, data);
    this->createAvatar(container, data);
    this->createNameLabel(container, data);

    return container;
}

Entity HealthBarWidgetLoader::createContainer(const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity container = this->factory.createBox(Rectangle{Position{0.f, 0.f}, data.fullSize});
    comp.add<HealthBarTag>(container, HealthBarTag{data.playerId, data.maxHealth, data.currentHealth});
    return container;
}

void HealthBarWidgetLoader::createBackground(Entity container, const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity background = this->factory.createRectangleShape(UIFactory::ShapeParams{Color{60, 60, 60, 255}},
        Rectangle{Position{0.f, 0.f}, data.barSize});
    this->factory.attachChild(container, background, data.barOffset);
    comp.get<RenderComponent>(background).zIndex = 0;
}

void HealthBarWidgetLoader::createBorder(Entity container, const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity border = this->factory.createRectangleShape(UIFactory::ShapeParams{Color::TRANSPARENT(), false},
        Rectangle{Position{0.f, 0.f}, data.barSize});
    this->factory.attachChild(container, border, data.barOffset);
    comp.get<RenderComponent>(border).zIndex = 100;
}

void HealthBarWidgetLoader::createSegments(Entity container, const BarData& data) const
{
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

        Entity fillSegment = this->factory.createRectangleShape(UIFactory::ShapeParams{colors.fill},
            Rectangle{Position{0.f, 0.f}, Dimension2D{segmentWidth, fillHeight}});
        this->factory.attachChild(container, fillSegment, {posX, data.barOffset.y});

        Entity shadowSegment = this->factory.createRectangleShape(UIFactory::ShapeParams{colors.shadow},
            Rectangle{Position{0.f, 0.f}, Dimension2D{segmentWidth, shadowHeight}});
        this->factory.attachChild(container, shadowSegment, {posX, data.barOffset.y + fillHeight});

        auto& comp = this->factory.world().components();

        comp.add<HealthBarSegmentComponent>(fillSegment, HealthBarSegmentComponent{
            rangeMaxHP, barTotalWidth, colors.fill, colors.shadow, i});
        comp.add<HealthBarSegmentComponent>(shadowSegment, HealthBarSegmentComponent{
            rangeMaxHP, barTotalWidth, colors.shadow, colors.shadow, i});

        comp.get<RenderComponent>(fillSegment).zIndex = 1 + i;
        comp.get<RenderComponent>(shadowSegment).zIndex = 1 + i;
    }
}

void HealthBarWidgetLoader::createBarFrameTexture(Entity container, const BarData& data) const
{
    if (data.paths.barFrame.empty()) return;

    Entity frame = this->factory.createImage(data.paths.barFrame, Rectangle{Position{0.f, 0.f}, data.barSize});
    this->factory.attachChild(container, frame, data.barOffset);
    this->factory.world().components().get<RenderComponent>(frame).zIndex = 200;
}

void HealthBarWidgetLoader::createAvatar(Entity container, const BarData& data) const
{
    Dimension2D avatarSize{data.avatarSize, data.avatarSize};

    auto& comp = this->factory.world().components();
    if (!data.paths.avatarFrame.empty())
    {
        Entity bg = this->factory.createImage(data.paths.avatarFrame, Rectangle{Position{0.f, 0.f}, avatarSize});
        this->factory.attachChild(container, bg, data.avatarOffset);
        comp.get<RenderComponent>(bg).zIndex = 50;
    }

    if (!data.paths.avatar.empty())
    {
        float innerSize = data.avatarSize * 0.8f;
        Position innerOffset = {
            data.avatarOffset.x + (data.avatarSize - innerSize) * 0.5f,
            data.avatarOffset.y + (data.avatarSize - innerSize) * 0.5f
        };

        Entity photo = this->factory.createImage(data.paths.avatar,
            Rectangle{Position{0.f, 0.f}, Dimension2D{innerSize, innerSize}});
        this->factory.attachChild(container, photo, innerOffset);
        comp.get<RenderComponent>(photo).zIndex = 60;
    }

    if (!data.paths.avatarBorder.empty())
    {
        Entity border = this->factory.createImage(data.paths.avatarBorder,
            Rectangle{Position{0.f, 0.f}, avatarSize});
        this->factory.attachChild(container, border, data.avatarOffset);
        comp.get<RenderComponent>(border).zIndex = 70;
    }
}

void HealthBarWidgetLoader::createNameLabel(Entity container, const BarData& data) const
{
    if (data.paths.characterName.empty()) return;

    auto& comp = this->factory.world().components();
    std::shared_ptr<Font> font = nullptr;
    try { font = this->factory.fontFactory().createFont(data.paths.font); }
    catch (const std::exception& e) { LOG_ERROR("Failed to load font for name: {}", e.what()); }

    UIFactory::TextParams textParams{
        .text       = data.paths.characterName,
        .fontSize   = data.fontSize,
        .color      = Color::WHITE(),
        .position   = data.nameOffset
    };

    Entity label = this->factory.createText(textParams, font);
    this->factory.attachChild(container, label, data.nameOffset);
    comp.get<RenderComponent>(label).zIndex = 80;
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
