#include "../../src/game/include/UIDrawer/UIDrawer.h"

#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/UISpriteComponent.h"
#include "../../src/domain/components/UITextComponent.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/components/VisualEffectsComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/RenderContext/RenderContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>

class UIDrawerFixture
{
public:
    class StubTexture : public Texture
    {
    public:
        int getWidth() const override { return 64; }
        int getHeight() const override { return 32; }
    };

    class StubFont : public Font
    {
    public:
        int getAscent(int) const override { return 12; }
        int getDescent(int) const override { return 4; }
        int getHeight(int) const override { return 16; }
    };

    class StubRenderer : public Renderer
    {
    public:
        int viewportCalls = 0;
        int textureCalls = 0;
        int fontCalls = 0;
        Viewport lastViewport;
        DrawTextureCommand lastTextureCommand;
        DrawFontCommand lastFontCommand;

        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand& cmd) override
        { this->textureCalls++; this->lastTextureCommand = cmd; }

        void drawFont(const DrawFontCommand& cmd) override
        { this->fontCalls++; this->lastFontCommand = cmd; }

        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}

        void setViewport(const Viewport& viewport) override
        { this->viewportCalls++; this->lastViewport = viewport; }
    };

    UIDrawerFixture() : drawer(this->bus, this->renderer, this->settings), context{ this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<RenderComponent>();
        components.registerComponent<UISpriteComponent>();
        components.registerComponent<UITextComponent>();
        components.registerComponent<UITransform>();
        components.registerComponent<VisualEffectsComponent>();
    }

    Entity createSpriteEntity(const Rectangle& rect, std::shared_ptr<Texture> texture)
    {
        Entity entity = this->world.entities().create();

        UITransform transform;
        transform.rect = rect;
        transform.rotation = 37.5f;
        transform.scale = { -2.0f, 3.0f };

        this->world.components().add<UITransform>(entity, transform);
        this->world.components().add<UISpriteComponent>(entity, UISpriteComponent{ texture, Color{ 10, 20, 30, 40 } });
        this->world.components().add<RenderComponent>(entity, RenderComponent{});

        return entity;
    }

    Entity createTextEntity(const Rectangle& rect, std::shared_ptr<Font> font)
    {
        Entity entity = this->world.entities().create();

        UITransform transform;
        transform.rect = rect;

        UITextComponent text;
        text.font = font;
        text.text = "Start";
        text.color = Color{ 5, 6, 7, 8 };
        text.fontSize = 24.0f;

        this->world.components().add<UITransform>(entity, transform);
        this->world.components().add<UITextComponent>(entity, text);
        this->world.components().add<RenderComponent>(entity, RenderComponent{});

        return entity;
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    GameSettings settings;
    UIDrawer drawer;
    RenderContext context;
};

TEST_CASE_METHOD(UIDrawerFixture, "UIDrawer configures the UI viewport on draw",
    "[unit][ui_drawer]"
) {
    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 1);
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

    this->createSpriteEntity(Rectangle{ Position{ 10.0f, 20.0f }, Dimension2D{ 120.0f, 40.0f } }, texture);
    this->createTextEntity(Rectangle{ Position{ 30.0f, 50.0f }, Dimension2D{ 80.0f, 24.0f } }, font);

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.textureCalls == 1);
    REQUIRE(this->renderer.lastTextureCommand.texture == texture.get());
    REQUIRE(this->renderer.lastTextureCommand.dest.position.x == Catch::Approx(10.0f));
    REQUIRE(this->renderer.lastTextureCommand.dest.position.y == Catch::Approx(20.0f));
    REQUIRE(this->renderer.lastTextureCommand.dest.size.width == Catch::Approx(120.0f));
    REQUIRE(this->renderer.lastTextureCommand.dest.size.height == Catch::Approx(40.0f));
    REQUIRE(this->renderer.lastTextureCommand.rotation == Catch::Approx(37.5f));
    REQUIRE(this->renderer.lastTextureCommand.flipX == true);
    REQUIRE(this->renderer.lastTextureCommand.flipY == false);
    REQUIRE(this->renderer.lastTextureCommand.tint == Color{ 10, 20, 30, 40 });
    REQUIRE(this->renderer.lastTextureCommand.order == 0);

    REQUIRE(this->renderer.fontCalls == 1);
    REQUIRE(this->renderer.lastFontCommand.font == font.get());
    REQUIRE(this->renderer.lastFontCommand.text == "Start");
    REQUIRE(this->renderer.lastFontCommand.dest.position.x == Catch::Approx(30.0f));
    REQUIRE(this->renderer.lastFontCommand.dest.position.y == Catch::Approx(50.0f));
    REQUIRE(this->renderer.lastFontCommand.dest.size.width == Catch::Approx(80.0f));
    REQUIRE(this->renderer.lastFontCommand.dest.size.height == Catch::Approx(24.0f));
    REQUIRE(this->renderer.lastFontCommand.fontSize == 24);
    REQUIRE(this->renderer.lastFontCommand.color == Color{ 5, 6, 7, 8 });
    REQUIRE(this->renderer.lastFontCommand.order == 0);
}

TEST_CASE_METHOD(UIDrawerFixture, "UIDrawer skips sprites without texture and texts without font or text",
    "[unit][ui_drawer]"
) {
    Entity sprite = this->world.entities().create();
    UITransform spriteTransform;
    spriteTransform.rect = Rectangle{ Position{ 1.0f, 2.0f }, Dimension2D{ 10.0f, 12.0f } };
    this->world.components().add<UITransform>(sprite, spriteTransform);
    this->world.components().add<UISpriteComponent>(sprite, UISpriteComponent{ nullptr, Color::WHITE() });
    this->world.components().add<RenderComponent>(sprite, RenderComponent{});

    Entity text = this->world.entities().create();
    UITransform textTransform;
    textTransform.rect = Rectangle{ Position{ 3.0f, 4.0f }, Dimension2D{ 20.0f, 22.0f } };
    this->world.components().add<UITransform>(text, textTransform);
    this->world.components().add<UITextComponent>(text, UITextComponent{});
    this->world.components().add<RenderComponent>(text, RenderComponent{});

    this->drawer.draw(this->context);

    REQUIRE(this->renderer.viewportCalls == 1);
    REQUIRE(this->renderer.textureCalls == 0);
    REQUIRE(this->renderer.fontCalls == 0);
}