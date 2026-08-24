#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/Color/Color.h"

#include "engine/utils/DataUtils/DataUtils.h"

#include <cstdlib>
#include <vector>

Entity HealthBarWidgetLoader::load(const DataNode& node, const UILoader::ParamMap& params)
{
    this->loadSegmentColors();

    BarData data;
    data.playerId       = static_cast<uint32_t>(std::stoul(params.at("playerId")));
    data.maxHealth      = std::stoi(params.at("maxHealth"));
    data.currentHealth  = std::stoi(params.at("currentHealth"));
    data.width          = node.getFloat("width", 300.f);
    data.height         = node.getFloat("height", 30.f);

    Entity container = this->createContainer(data);
    this->createBackground(container, data);
    this->createBorder(container, data);
    this->createSegments(container, data);

    return container;
}

Entity HealthBarWidgetLoader::createContainer(const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity container = this->factory.createBox(Rectangle{Position{0.f, 0.f}, Dimension2D{data.width, data.height}});
    comp.add<HealthBarTag>(container, HealthBarTag{data.playerId, data.maxHealth, data.currentHealth});
    return container;
}

void HealthBarWidgetLoader::createBackground(Entity container, const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity background = this->factory.createRectangleShape(UIFactory::ShapeParams{
        Color{60, 60, 60, 255}}, Rectangle{Position{0.f, 0.f}, Dimension2D{data.width, data.height}});
    this->factory.attachChild(container, background, {0.f, 0.f});
    comp.get<RenderComponent>(background).zIndex = 0;
}

void HealthBarWidgetLoader::createBorder(Entity container, const BarData& data) const
{
    auto& comp = this->factory.world().components();
    Entity border = this->factory.createRectangleShape(
        UIFactory::ShapeParams{Color::TRANSPARENT(), false},
        Rectangle{Position{0.f, 0.f}, Dimension2D{data.width, data.height}});
    this->factory.attachChild(container, border, {0.f, 0.f});
    comp.get<RenderComponent>(border).zIndex = 2;
}



void HealthBarWidgetLoader::createSegments(Entity container, const BarData& data) const
{
    const float maxSegmentHP = 100.f;
    int numSegments = (data.maxHealth + static_cast<int>(maxSegmentHP) - 1) / static_cast<int>(maxSegmentHP);
    float segmentMaxWidth = data.width / numSegments;
    float remainingHP = static_cast<float>(data.currentHealth);

    const float shadowHeight = data.height * HealthBarWidgetLoader::SHADOW_HEIGHT_RATIO;
    const float fillHeight = data.height - shadowHeight;

    for (int i = 0; i < numSegments; ++i)
    {
        SegmentParams params;
        params.remainingHP = remainingHP;
        params.maxHP = maxSegmentHP;
        params.maxWidth = segmentMaxWidth;
        SegmentData seg = this->computeSegment(i, params);
        remainingHP -= seg.hp;

        float width = (seg.hp / seg.maxHP) * seg.maxWidth;
        SegmentColor colors = this->getSegmentColor(i);

        Entity fillSegment = this->factory.createRectangleShape(
            UIFactory::ShapeParams{colors.fill},
            Rectangle{Position{0.f, 0.f}, Dimension2D{width, fillHeight}});
        this->factory.attachChild(container, fillSegment, Position{seg.index * seg.maxWidth, 0.f});

        Entity shadowSegment = this->factory.createRectangleShape(
            UIFactory::ShapeParams{colors.shadow},
            Rectangle{Position{0.f, 0.f}, Dimension2D{width, shadowHeight}});
        this->factory.attachChild(container, shadowSegment, Position{seg.index * seg.maxWidth, fillHeight});

        auto& comp = this->factory.world().components();

        comp.add<HealthBarSegmentComponent>(fillSegment, HealthBarSegmentComponent{
            seg.maxHP, seg.maxWidth, colors.fill, colors.shadow, i});

        comp.add<HealthBarSegmentComponent>(shadowSegment, HealthBarSegmentComponent{
            seg.maxHP, seg.maxWidth, colors.shadow, colors.shadow, i});

        comp.get<RenderComponent>(fillSegment).zIndex = 1;
        comp.get<RenderComponent>(shadowSegment).zIndex = 1;
    }
}

HealthBarWidgetLoader::SegmentData HealthBarWidgetLoader::computeSegment(int index, const SegmentParams& params) const
{
    SegmentData seg;
    seg.index       = index;
    seg.maxHP       = params.maxHP;
    seg.maxWidth    = params.maxWidth;
    seg.hp          = std::min(params.remainingHP, params.maxHP);
    return seg;
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
