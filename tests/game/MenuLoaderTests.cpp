#include "../../src/game/include/MenuLoader/MenuLoader.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/Window/Window.h"
#include "../../src/engine/value_objects/GameSettings/GameSettings.h"

#include "../../src/game/ui_actions/QuitAction.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"
#include "../stubs/StubEngine.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubPlatformFactory.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubSceneManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTextureLoader.h"
#include "../stubs/StubWindow.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class MenuLoaderFixture
{
public:
    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    StubDataParser dataParser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;
    EventBus eventBus;
    std::shared_ptr<UIActionFactory> actionFactory;

    MenuLoaderFixture()
    {
        this->engine = std::make_unique<StubEngine>(this->window, this->settings);
        this->engine->setRenderer(this->renderer);

        this->textureFactory = std::make_unique<StubTextureFactory>();
        this->fontFactory = std::make_unique<StubFontFactory>();
        this->textureLoader = std::make_unique<StubTextureLoader>(*this->textureFactory);

        this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
            .window             = this->window,
            .parser             = this->dataParser,
            .resourceManager    = this->resourceManager,
            .textureLoader      = *this->textureLoader,
            .renderer           = this->renderer,
            .eventBus           = this->engine->events(),
            .settings           = this->settings,
            .engine             = *this->engine,
            .fontFactory        = *this->fontFactory,
            .textureFactory     = *this->textureFactory
        });

        this->sceneManager = std::make_unique<SceneManager>(*this->factory, *this->engine);
        this->engine->setSceneManager(*this->sceneManager);

        UIActionFactory::Config uiCfg{this->engine->events(), this->engine->scenes()};
        this->actionFactory = std::make_shared<UIActionFactory>(uiCfg);
    }

    std::unique_ptr<StubDataNode> makeEmptyObject() const { return std::make_unique<StubDataNode>(); }

    std::unique_ptr<StubDataNode> makeRect(float x, float y, float width, float height) const
    {
        auto size = std::make_unique<StubDataNode>();
        size->setFloat("w", width);
        size->setFloat("h", height);
        auto position = std::make_unique<StubDataNode>();
        position->setFloat("x", x);
        position->setFloat("y", y);
        auto rect = std::make_unique<StubDataNode>();
        rect->setObject("position", std::move(position));
        rect->setObject("size", std::move(size));
        return rect;
    }

    std::unique_ptr<StubDataNode> makeColor(int r, int g, int b, int a) const
    {
        auto color = std::make_unique<StubDataNode>();
        color->setInt("r", r);
        color->setInt("g", g);
        color->setInt("b", b);
        color->setInt("a", a);
        return color;
    }

    std::unique_ptr<StubDataNode> makeAction(const std::string& type) const
    {
        auto action = std::make_unique<StubDataNode>();
        action->setString("type", type);
        return action;
    }

    std::unique_ptr<StubDataNode> makeElement(const std::string& type) const
    {
        auto elem = std::make_unique<StubDataNode>();
        elem->setString("type", type);
        elem->setObject("rect", this->makeEmptyObject());
        elem->setObject("margin", this->makeEmptyObject());
        elem->setObject("padding", this->makeEmptyObject());
        elem->setObject("border", this->makeEmptyObject());
        elem->setObject("borderRadius", this->makeEmptyObject());
        return elem;
    }

    std::unique_ptr<StubDataNode> makeMenuRoot() const
    {
        auto panel = this->makeElement("Panel");
        panel->setString("id", "menu_panel");
        panel->setObject("rect", this->makeRect(10.f, 20.f, 640.f, 480.f));

        auto title = this->makeElement("Text");
        title->setString("id", "menu_title");
        title->setString("text", "Battle Stars");
        title->setFloat("fontSize", 36.f);
        title->setString("font", "assets/fonts/title.ttf");
        title->setObject("color", this->makeColor(12, 34, 56, 78));

        auto button = this->makeElement("Button");
        button->setString("id", "start_button");
        button->setString("text", "Start");
        button->setFloat("fontSize", 28.f);
        button->setString("font", "assets/fonts/button.ttf");
        button->setObject("color", this->makeColor(200, 210, 220, 255));
        button->setObject("action", this->makeAction("Quit"));

        auto image = this->makeElement("Image");
        image->setString("id", "logo");
        image->setString("texture", "assets/ui/logo.png");

        auto root = std::make_unique<StubDataNode>();
        root->setString("id", "main_menu");
        std::vector<std::unique_ptr<DataNode>> elements;
        elements.push_back(std::move(panel));
        elements.push_back(std::move(title));
        elements.push_back(std::move(button));
        elements.push_back(std::move(image));
        root->setArray("elements", std::move(elements));
        return root;
    }
};

TEST_CASE_METHOD(MenuLoaderFixture, "MenuLoader parses menu elements and forwards path to parser",
    "[unit][menu_loader]"
) {
    StubDataParser fakeParser;
    fakeParser.registerNode("assets/ui/main_menu.json", this->makeMenuRoot());

    MenuLoader loader(fakeParser, *this->actionFactory);
    const auto data = loader.load("assets/ui/main_menu.json");

    REQUIRE(data.id == "main_menu");
    REQUIRE(data.elements.size() == 4);

    const auto* panel = dynamic_cast<PanelElement*>(data.elements[0].get());
    REQUIRE(panel != nullptr);
    REQUIRE(panel->id == "menu_panel");
    REQUIRE(panel->rect.position.x == 10.f);
    REQUIRE(panel->rect.position.y == 20.f);
    REQUIRE(panel->rect.size.width == 640.f);
    REQUIRE(panel->rect.size.height == 480.f);

    const auto* title = dynamic_cast<TextElement*>(data.elements[1].get());
    REQUIRE(title != nullptr);
    REQUIRE(title->id == "menu_title");
    REQUIRE(title->text == "Battle Stars");
    REQUIRE(title->fontSize == 36.f);
    REQUIRE(title->fontPath == "assets/fonts/title.ttf");
    REQUIRE(title->color.r == 12);
    REQUIRE(title->color.g == 34);
    REQUIRE(title->color.b == 56);
    REQUIRE(title->color.a == 78);

    const auto* button = dynamic_cast<ButtonElement*>(data.elements[2].get());
    REQUIRE(button != nullptr);
    REQUIRE(button->id == "start_button");
    REQUIRE(button->text == "Start");
    REQUIRE(button->fontSize == 28.f);
    REQUIRE(button->fontPath == "assets/fonts/button.ttf");
    REQUIRE(button->action != nullptr);
    REQUIRE(dynamic_cast<QuitAction*>(button->action.get()) != nullptr);

    int quitCount = 0;
    this->engine->events().subscribe<QuitEvent>([&](const QuitEvent&) { ++quitCount; });
    button->action->execute();
    REQUIRE(quitCount == 1);

    const auto* image = dynamic_cast<ImageElement*>(data.elements[3].get());
    REQUIRE(image != nullptr);
    REQUIRE(image->id == "logo");
    REQUIRE(image->imagePath == "assets/ui/logo.png");
}

TEST_CASE_METHOD(MenuLoaderFixture, "MenuLoader throws on unknown element type", "[unit][menu_loader]")
{
    auto unknown = std::make_unique<StubDataNode>();
    unknown->setString("type", "SomethingElse");

    auto root = std::make_unique<StubDataNode>();
    root->setString("id", "main_menu");
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(unknown));
    root->setArray("elements", std::move(elements));

    StubDataParser fakeParser;
    fakeParser.registerNode("assets/ui/main_menu.json", std::move(root));

    MenuLoader loader(fakeParser, *this->actionFactory);
    REQUIRE_THROWS_AS(loader.load("assets/ui/main_menu.json"), std::runtime_error);
}
