#ifndef ui_layout_system_h
#define ui_layout_system_h

#include "domain/components/FlexContainer.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/System/System.h"

#include <vector>

class UILayoutSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    struct Item { UpdateContext& ctx; Entity& entity; };
    struct ItemGap { float gap = 1.f; bool isColumn = false; };
    
    struct DistributionParams { float mainPos = 0.f, freeSpace = 0.f; FlexContainer::JustifyContent justify; };
    struct AlignmentParams { float crossPos = 0.f, crossSize = 0.f; bool isColumn = false; FlexContainer::AlignItems align; };

    std::vector<Entity> collectChildren(Item item);
    void applyPadding(Item item, Rectangle& innerRect) const;
    float calculateFreeSpace(Item item, ItemGap& itemGap, float mainSize);
    void distributeMainAxis(Item item, ItemGap& itemGap, DistributionParams& params);
    void positionItemMenu(Item item, float& cursor, ItemGap& itemGap);
    void alignItemCross(Item item, AlignmentParams& params);
    void layoutContainer(Item item);
};

#endif // ui_layout_system_h
