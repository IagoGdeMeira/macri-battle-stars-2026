#include "game/utils/ComponentRegistry/HUDComponentRegistry.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/FontEffectsComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIIDComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("HUDComponentRegistry registers every HUD component", "[unit][hud_component_registry]")
{
    ComponentManager comp;

    HUDComponentRegistry::registerAll(comp);

    REQUIRE(comp.storage<ActiveComponent>() != nullptr);
    REQUIRE(comp.storage<BoxModel>() != nullptr);
    REQUIRE(comp.storage<ChildrenComponent>() != nullptr);
    REQUIRE(comp.storage<CircleEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<CircleShapeComponent>() != nullptr);
    REQUIRE(comp.storage<FlexContainer>() != nullptr);
    REQUIRE(comp.storage<FlexItem>() != nullptr);
    REQUIRE(comp.storage<FontEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<HealthBarSegmentComponent>() != nullptr);
    REQUIRE(comp.storage<HealthBarTag>() != nullptr);
    REQUIRE(comp.storage<HUDEntityTag>() != nullptr);
    REQUIRE(comp.storage<LayoutDirtyComponent>() != nullptr);
    REQUIRE(comp.storage<LocalTransform>() != nullptr);
    REQUIRE(comp.storage<OrientationComponent>() != nullptr);
    REQUIRE(comp.storage<ParentComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleEffectsComponent>() != nullptr);
    REQUIRE(comp.storage<RectangleShapeComponent>() != nullptr);
    REQUIRE(comp.storage<RenderComponent>() != nullptr);
    REQUIRE(comp.storage<RoundTimerTag>() != nullptr);
    REQUIRE(comp.storage<TransformComponent>() != nullptr);
    REQUIRE(comp.storage<UIIDComponent>() != nullptr);
    REQUIRE(comp.storage<UILayoutMetricsComponent>() != nullptr);
    REQUIRE(comp.storage<UISpriteComponent>() != nullptr);
    REQUIRE(comp.storage<UITextComponent>() != nullptr);
}

TEST_CASE("HUDComponentRegistry can be invoked more than once safely", "[unit][hud_component_registry]")
{
    ComponentManager comp;

    REQUIRE_NOTHROW(HUDComponentRegistry::registerAll(comp));
    REQUIRE_NOTHROW(HUDComponentRegistry::registerAll(comp));
}
