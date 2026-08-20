#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Color/Color.h"

#include <cstdlib>

Entity HealthBarWidgetLoader::load(const DataNode& node, const UILoader::ParamMap& params)
{
    auto& comp = this->factory.world().components();
    auto& entities = this->factory.world().entities();

    const float totalWidth = node.getFloat("width", 300.f);
    const float barHeight = node.getFloat("height", 30.f);
    const float maxSegmentHP = 100.f;

    uint32_t playerId = static_cast<uint32_t>(std::stoul(params.at("playerId")));
    int maxHealth = std::stoi(params.at("maxHealth"));
    int currentHealth = std::stoi(params.at("currentHealth"));

    int numSegments = (maxHealth + static_cast<int>(maxSegmentHP) - 1) / static_cast<int>(maxSegmentHP);
    float segmentMaxWidth = totalWidth / numSegments;

    Entity container = this->factory.createBox(Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}});
    comp.add<HealthBarTag>(container, HealthBarTag{playerId, maxHealth, currentHealth});

    Entity background = entities.create();
    {
        comp.add<ParentComponent>(background, ParentComponent{container});
        comp.add<LocalTransform>(background, LocalTransform{Position{0.f, 0.f}});
        comp.add<TransformComponent>(background, TransformComponent{Position{0.f, 0.f}});
        comp.add<UILayoutMetricsComponent>(background, UILayoutMetricsComponent{Dimension2D{totalWidth, barHeight}});
        comp.add<RectangleShapeComponent>(background, RectangleShapeComponent{
            Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}},
            Color{60, 60, 60, 255}, true, 0, 0});
        comp.add<RenderComponent>(background, RenderComponent{0, 0});
    }

    Entity border = entities.create();
    {
        comp.add<ParentComponent>(border, ParentComponent{container});
        comp.add<LocalTransform>(border, LocalTransform{Position{0.f, 0.f}});
        comp.add<TransformComponent>(border, TransformComponent{Position{0.f, 0.f}});
        comp.add<UILayoutMetricsComponent>(border, UILayoutMetricsComponent{Dimension2D{totalWidth, barHeight}});
        comp.add<RectangleShapeComponent>(border, RectangleShapeComponent{
            Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}},
            Color::TRANSPARENT(), false, 0, 2});
        comp.add<RenderComponent>(border, RenderComponent{0, 2});
    }

    float remainingHP = static_cast<float>(currentHealth);
    for (int i = 0; i < numSegments; ++i)
    {
        float segmentHP = std::min(remainingHP, maxSegmentHP);
        remainingHP -= segmentHP;

        bool isLast = (i == numSegments - 1);
        Color fillColor = isLast ? Color{255, 200, 0, 255} : Color{0, 200, 0, 255};

        float width = (segmentHP / maxSegmentHP) * segmentMaxWidth;

        Entity segment = entities.create();
        comp.add<ParentComponent>(segment, ParentComponent{container});
        comp.add<LocalTransform>(segment, LocalTransform{Position{i * segmentMaxWidth, 0.f}});
        comp.add<TransformComponent>(segment, TransformComponent{Position{0.f, 0.f}});
        comp.add<UILayoutMetricsComponent>(segment, UILayoutMetricsComponent{Dimension2D{width, barHeight}});
        comp.add<RectangleShapeComponent>(segment, RectangleShapeComponent{
            Rectangle{Position{0.f, 0.f}, Dimension2D{width, barHeight}},
            fillColor, true, 0, 1});
        comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{maxSegmentHP, segmentMaxWidth});
        comp.add<RenderComponent>(segment, RenderComponent{0, 1});
    }

    return container;
}
