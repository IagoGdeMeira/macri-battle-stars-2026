#include "ComponentRegistry/HUDComponentRegistry.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentTag.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/components/UITransform.h"
#include "domain/include/ComponentManager/ComponentManager.h"

void HUDComponentRegistry::registerAll(ComponentManager& comp)
{
    comp.registerComponent<ActiveComponent>();
    comp.registerComponent<BoxModel>();
    comp.registerComponent<ChildrenComponent>();
    comp.registerComponent<FlexContainer>();
    comp.registerComponent<HealthBarTag>();
    comp.registerComponent<HealthBarSegmentTag>();
    comp.registerComponent<HUDEntityTag>();
    comp.registerComponent<ParentComponent>();
    comp.registerComponent<RenderComponent>();
    comp.registerComponent<UISpriteComponent>();
    comp.registerComponent<UITextComponent>();
    comp.registerComponent<UITransform>();
}
