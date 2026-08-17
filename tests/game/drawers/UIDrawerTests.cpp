#include "game/drawers/UIDrawer/UIDrawer.h"

#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/FontEffectsComponent.h"
#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include "StubRenderer.h"
#include "StubTexture.h"
#include "StubFont.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

class UIDrawerFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    GameSettings settings;
    UIDrawer drawer;
    RenderContext context;

    UIDrawerFixture() : drawer(this->bus, this->renderer, this->settings), context{ this->world, this->bus }
    {
        auto &comp = this->world.components();
        comp.registerComponent<CircleEffectsComponent>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<FontEffectsComponent>();
        comp.registerComponent<RectangleEffectsComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TextureEffectsComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
        comp.registerComponent<UISpriteComponent>();
        comp.registerComponent<UITextComponent>();
    }

    Entity createSpriteEntity(const Rectangle& rect, std::shared_ptr<Texture> texture)
    {
        Entity entity = this->world.entities().create();

        TransformComponent tc;
        tc.position = rect.position;
        tc.rotation = 37.5f;
        tc.scale = {-2.f, 3.f};

        UILayoutMetricsComponent layout;
        layout.size = rect.size;

        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, tc);
        comp.add<UILayoutMetricsComponent>(entity, layout);
        comp.add<UISpriteComponent>(entity, UISpriteComponent{texture, Color{10, 20, 30, 40}});
        comp.add<RenderComponent>(entity, RenderComponent{});

        return entity;
    }

    Entity createTextEntity(const Rectangle& rect, std::shared_ptr<Font> font)
    {
        Entity entity = this->world.entities().create();

        TransformComponent tc;
        tc.position = rect.position;

        UILayoutMetricsComponent layout;
        layout.size = rect.size;

        UITextComponent text;
        text.font = font;
        text.text = "Start";
        text.color = Color{5, 6, 7, 8};
        text.fontSize = 24.f;

        auto& comp = this->world.components();
        comp.add<TransformComponent>(entity, tc);
        comp.add<UILayoutMetricsComponent>(entity, layout);
        comp.add<UITextComponent>(entity, text);
        comp.add<RenderComponent>(entity, RenderComponent{});

        return entity;
    }
};

TEST_CASE_METHOD(UIDrawerFixture, "UIDrawer configures the UI viewport on draw",
    "[unit][ui_drawer]"
) {
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.calls.setViewport == 1);
    REQUIRE(this->renderer.lastViewport.x == 0);
    REQUIRE(this->renderer.lastViewport.y == 0);
    REQUIRE(this->renderer.lastViewport.width == 800);
    REQUIRE(this->renderer.lastViewport.height == 600);
}

TEST_CASE_METHOD(UIDrawerFixture, "UIDrawer draws sprite and text commands from UI components",
    "[unit][ui_drawer]"
) {
    auto texture = std::make_shared<StubTexture>();
    auto font = std::make_shared<StubFont>();

    this->createSpriteEntity(Rectangle{Position{10.f, 20.f}, Dimension2D{120.f, 40.f}}, texture);
    this->createTextEntity(Rectangle{Position{30.f, 50.f}, Dimension2D{80.f, 24.f}}, font);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    const auto& texCmd = this->renderer.textureCalls[0];
    REQUIRE(texCmd.texture == texture);
    REQUIRE(texCmd.dest.position.x == Catch::Approx(10.f));
    REQUIRE(texCmd.dest.position.y == Catch::Approx(20.f));
    REQUIRE(texCmd.dest.size.width == Catch::Approx(120.f));
    REQUIRE(texCmd.dest.size.height == Catch::Approx(40.f));
    REQUIRE(texCmd.rotation == Catch::Approx(37.5f));
    REQUIRE(texCmd.flipX == true);
    REQUIRE(texCmd.flipY == false);
    REQUIRE(texCmd.tint == Color{10, 20, 30, 40});
    REQUIRE(texCmd.order == 0);

    REQUIRE(this->renderer.fontCalls.size() == 1);
    const auto& fontCmd = this->renderer.fontCalls[0];
    REQUIRE(fontCmd.font == font.get());
    REQUIRE(fontCmd.text == "Start");
    REQUIRE(fontCmd.dest.position.x == Catch::Approx(30.f));
    REQUIRE(fontCmd.dest.position.y == Catch::Approx(50.f));
    REQUIRE(fontCmd.dest.size.width == Catch::Approx(80.f));
    REQUIRE(fontCmd.dest.size.height == Catch::Approx(24.f));
    REQUIRE(fontCmd.fontSize == 24);
    REQUIRE(fontCmd.color == Color{5, 6, 7, 8});
    REQUIRE(fontCmd.order == 0);
}

TEST_CASE_METHOD(UIDrawerFixture, "UIDrawer skips sprites without texture and texts without font or text",
    "[unit][ui_drawer]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity sprite = entities.create();

    TransformComponent stc;
    stc.position = {1.f, 2.f};
    UILayoutMetricsComponent spriteLayout;
    spriteLayout.size = {10.f, 12.f};

    comp.add<TransformComponent>(sprite, stc);
    comp.add<UILayoutMetricsComponent>(sprite, spriteLayout);
    comp.add<UISpriteComponent>(sprite, UISpriteComponent{ nullptr, Color::WHITE() });
    comp.add<RenderComponent>(sprite, RenderComponent{});

    Entity text = entities.create();
    TransformComponent ttc;
    UILayoutMetricsComponent textLayout;
    ttc.position = {3.f, 4.f};
    textLayout.size = {20.f, 22.f};

    comp.add<TransformComponent>(text, ttc);
    comp.add<UILayoutMetricsComponent>(text, textLayout);
    comp.add<UITextComponent>(text, UITextComponent{});
    comp.add<RenderComponent>(text, RenderComponent{});

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.textureCalls.empty());
    REQUIRE(this->renderer.fontCalls.empty());
}
