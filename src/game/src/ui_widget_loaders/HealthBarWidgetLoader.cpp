#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Color/Color.h"

#include <cstdlib>

Entity HealthBarWidgetLoader::load(const DataNode& node, const UILoader::ParamMap& params)
{
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
    Entity background = this->factory.createRectangleShape(UIFactory::ShapeParams{Color{60, 60, 60, 255}},
        Rectangle{Position{0.f, 0.f}, Dimension2D{data.width, data.height}});
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

    for (int i = 0; i < numSegments; ++i)
    {
        SegmentParams params;
        params.remainingHP = remainingHP;
        params.maxHP = maxSegmentHP;
        params.maxWidth = segmentMaxWidth;
        SegmentData seg = this->computeSegment(i, params);
        remainingHP -= seg.hp;

        float width = (seg.hp / seg.maxHP) * seg.maxWidth;
        Color fillColor = (i == numSegments - 1) ? Color{255, 200, 0, 255} : Color{0, 200, 0, 255};

        Entity segment = this->factory.createRectangleShape(UIFactory::ShapeParams{fillColor},
            Rectangle{Position{0.f, 0.f}, Dimension2D{width, data.height}});
        this->factory.attachChild(container, segment, Position{seg.index * seg.maxWidth, 0.f});

        auto& comp = this->factory.world().components();
        comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{
            seg.maxHP, seg.maxWidth, fillColor, Color{0, 90, 0, 255}});
        comp.get<RenderComponent>(segment).zIndex = 1;
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
