#include "HealthBarWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/include/World/World.h"

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

    Entity container = this->factory.createPanel(Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}});
    comp.add<HealthBarTag>(container, HealthBarTag{playerId, maxHealth, currentHealth});
    comp.add<RenderComponent>(container, RenderComponent{0, 0});

    Entity background = this->factory.createImage("", Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}});
    comp.get<UISpriteComponent>(background).tint = Color{60, 60, 60, 255};
    comp.add<ParentComponent>(background, ParentComponent{container});

    Entity border = this->factory.createImage("", Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}});
    comp.get<UISpriteComponent>(border).tint = Color::TRANSPARENT();
    comp.add<ParentComponent>(border, ParentComponent{container});

    float remainingHP = static_cast<float>(currentHealth);
    for (int i = 0; i < numSegments; ++i)
    {
        float segmentHP = std::min(remainingHP, maxSegmentHP);
        remainingHP -= segmentHP;

        bool isLast = (i == numSegments - 1);
        Color fillColor = isLast ? Color{255, 200, 0, 255} : Color{0, 200, 0, 255};

        Entity segment = this->factory.createImage("", Rectangle{
            Position{i * segmentMaxWidth, 0.f},
            Dimension2D{(segmentHP / maxSegmentHP) * segmentMaxWidth, barHeight}});
        comp.get<UISpriteComponent>(segment).tint = fillColor;
        comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{maxSegmentHP, segmentMaxWidth});
        comp.add<ParentComponent>(segment, ParentComponent{container});
    }

    return container;
}
