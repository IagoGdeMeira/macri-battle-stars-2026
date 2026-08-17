#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Color/Color.h"

#include <cstdlib>

Entity HealthBarWidgetLoader::load(const DataNode& node, const UILoader::ParamMap& params)
{
    auto& comp = this->factory.world().components();

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

    Entity background = this->factory.createRectangleShapeChild(
        container, Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight},
        UIFactory::ShapeParams{Color{60, 60, 60, 255}}
    );
    comp.get<RenderComponent>(background).zIndex = 0;

    Entity border = this->factory.createRectangleShapeChild(
        container, Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight},
        UIFactory::ShapeParams{Color::TRANSPARENT(), false}
    );
    comp.get<RenderComponent>(border).zIndex = 2;

    float remainingHP = static_cast<float>(currentHealth);
    for (int i = 0; i < numSegments; ++i)
    {
        float segmentHP = std::min(remainingHP, maxSegmentHP);
        remainingHP -= segmentHP;

        bool isLast = (i == numSegments - 1);
        Color fillColor = isLast ? Color{255, 200, 0, 255} : Color{0, 200, 0, 255};

        Entity segment = this->factory.createRectangleShapeChild(
            container,
            Position{i * segmentMaxWidth, 0.f},
            Dimension2D{(segmentHP / maxSegmentHP) * segmentMaxWidth, barHeight},
            UIFactory::ShapeParams{fillColor}
        );
        comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{maxSegmentHP, segmentMaxWidth});
        comp.get<RenderComponent>(segment).zIndex = 1;
    }

    return container;
}
