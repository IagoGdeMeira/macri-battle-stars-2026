#include "game/utils/ComponentRegistry/HUDComponentRegistry.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentTag.h"
#include "domain/components/HudEntityTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/components/UITransform.h"
#include "domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("HUDComponentRegistry registers every HUD component", "[unit][hud_component_registry]")
{
    ComponentManager comp;

    HUDComponentRegistry::registerAll(comp);

    REQUIRE(comp.storage<ActiveComponent>() != nullptr);
    REQUIRE(comp.storage<BoxModel>() != nullptr);
    REQUIRE(comp.storage<ChildrenComponent>() != nullptr);
    REQUIRE(comp.storage<FlexContainer>() != nullptr);
    REQUIRE(comp.storage<HealthBarTag>() != nullptr);
    REQUIRE(comp.storage<HealthBarSegmentTag>() != nullptr);
    REQUIRE(comp.storage<HUDEntityTag>() != nullptr);
    REQUIRE(comp.storage<ParentComponent>() != nullptr);
    REQUIRE(comp.storage<RenderComponent>() != nullptr);
    REQUIRE(comp.storage<UISpriteComponent>() != nullptr);
    REQUIRE(comp.storage<UITextComponent>() != nullptr);
    REQUIRE(comp.storage<UITransform>() != nullptr);
}

TEST_CASE("HUDComponentRegistry can be invoked more than once safely", "[unit][hud_component_registry]")
{
    ComponentManager comp;

    REQUIRE_NOTHROW(HUDComponentRegistry::registerAll(comp));
    REQUIRE_NOTHROW(HUDComponentRegistry::registerAll(comp));
}
