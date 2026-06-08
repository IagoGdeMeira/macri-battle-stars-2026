#include "../../src/game/include/MenuLoader/MenuLoader.h"

#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../src/engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/Scene/Scene.h"
#include "../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../src/engine/include/Viewport/Viewport.h"
#include "../../src/engine/include/Window/Window.h"

#include "../../src/game/ui_actions/QuitAction.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class MenuLoaderFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& w, int& h) override { w = 800; h = 600; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}
        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}
    };

    class StubDataParser : public DataParser
    {
    public:
        std::unique_ptr<DataNode> parse(const std::string&) const override { return nullptr; }
    };

    class StubResourceManager : public ResourceManager
    {
    public:
        StubResourceManager() : ResourceManager(this->pool) {}

    private:
        ThreadPool pool{1};
    };

    class StubTextureFactory : public ITextureFactory
    {
    public:
        std::shared_ptr<Texture> createTexture(const std::string&) override { return nullptr; }
    };

    class StubFontFactory : public IFontFactory
    {
    public:
        std::shared_ptr<Font> createFont(const std::string&) override { return nullptr; }
    };

    class StubPlatformFactory : public IPlatformFactory
    {
    public:
        std::unique_ptr<Window> createWindow(int, int, const std::string&) override { return nullptr; }
        std::unique_ptr<Renderer> createRenderer(Window&) override { return nullptr; }
        InputAdapterPtr createKeyboardAdapter(EventBus&, const InputContext&) override { return nullptr; }
        InputAdapterPtr createMouseAdapter(EventBus&) override { return nullptr; }
        std::vector<InputAdapterPtr> createGamepadAdapters(EventBus&, uint32_t) override { return {}; }
        InputAdapterPtr createSystemAdapter(EventBus&) override { return nullptr; }
        std::unique_ptr<IPlatformEventProvider> createEventProvider() override { return nullptr; }
        std::unique_ptr<ITextureFactory> createTextureFactory(Renderer&) override { return nullptr; }
        std::unique_ptr<ISoundFactory> createSoundFactory() override { return nullptr; }
        std::unique_ptr<IMusicFactory> createMusicFactory() override { return nullptr; }
        std::unique_ptr<IFontFactory> createFontFactory() override { return nullptr; }
    };

    class StubTextureLoader : public TextureLoader
    {
    public:
        StubTextureLoader(ITextureFactory& factory) : TextureLoader(factory) {}
    };

    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }
        void setFloat(const std::string& key, float value) { this->floats[key] = value; }
        void setBool(const std::string& key, bool value) { this->bools[key] = value; }

        void setObject(const std::string& key, std::unique_ptr<DataNode> value)
        {
            auto* typed = dynamic_cast<Node*>(value.get());
            if (!typed) throw std::runtime_error("Unexpected node type");
            this->objects[key] = *typed;
        }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<Node> converted;
            converted.reserve(value.size());
            for (auto& node : value)
            {
                auto* typed = dynamic_cast<Node*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type");
                converted.push_back(*typed);
            }
            this->arrays[key] = std::move(converted);
        }

        bool has(const std::string& key) const override
        {
            if (this->strings.contains(key)) return true;
            if (this->ints.contains(key)) return true;
            if (this->floats.contains(key)) return true;
            if (this->bools.contains(key)) return true;
            if (this->objects.contains(key)) return true;
            if (this->arrays.contains(key)) return true;
            return false;
        }

        std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override
        {
            auto it = this->strings.find(key);
            return (it != this->strings.end()) ? it->second : fallback;
        }

        int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
        {
            auto it = this->ints.find(key);
            return (it != this->ints.end()) ? it->second : fallback;
        }

        float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
        {
            auto it = this->floats.find(key);
            return (it != this->floats.end()) ? it->second : fallback;
        }

        bool getBool(const std::string &key, const bool &fallback = DataNode::defaultBoolFallback) const override
        {
            auto it = this->bools.find(key);
            return (it != this->bools.end()) ? it->second : fallback;
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string &key) const override
        {
            auto it = this->arrays.find(key);
            if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);
            
            std::vector<std::unique_ptr<DataNode>> out;
            for (const auto& node : it->second) out.push_back(std::make_unique<Node>(node));
            return out;
        }

        std::unique_ptr<DataNode> getObject(const std::string &key) const override
        {
            auto it = this->objects.find(key);
            if (it == this->objects.end()) return nullptr;
            return std::make_unique<Node>(it->second);
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, Node> objects;
        std::unordered_map<std::string, std::vector<Node>> arrays;
    };

    class Parser : public DataParser
    {
    public:
        explicit Parser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}
        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string &filePath) const override
        {
            this->lastPath = filePath;
            auto *typed = dynamic_cast<Node *>(this->root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type");
            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    static std::unique_ptr<DataNode> makeEmptyObject() { return std::make_unique<Node>(); }

    static std::unique_ptr<DataNode> makeRect(float x, float y, float width, float height)
    {
        auto size = std::make_unique<Node>();
        size->setFloat("w", width);
        size->setFloat("h", height);
        auto position = std::make_unique<Node>();
        position->setFloat("x", x);
        position->setFloat("y", y);
        auto rect = std::make_unique<Node>();
        rect->setObject("position", std::move(position));
        rect->setObject("size", std::move(size));
        return rect;
    }

    static std::unique_ptr<DataNode> makeColor(int r, int g, int b, int a)
    {
        auto color = std::make_unique<Node>();
        color->setInt("r", r);
        color->setInt("g", g);
        color->setInt("b", b);
        color->setInt("a", a);
        return color;
    }

    static std::unique_ptr<DataNode> makeAction(const std::string &type)
    {
        auto action = std::make_unique<Node>();
        action->setString("type", type);
        return action;
    }

    static std::unique_ptr<DataNode> makeElement(const std::string &type)
    {
        auto elem = std::make_unique<Node>();
        elem->setString("type", type);
        elem->setObject("rect", MenuLoaderFixture::makeEmptyObject());
        elem->setObject("margin", MenuLoaderFixture::makeEmptyObject());
        elem->setObject("padding", MenuLoaderFixture::makeEmptyObject());
        elem->setObject("border", MenuLoaderFixture::makeEmptyObject());
        elem->setObject("borderRadius", MenuLoaderFixture::makeEmptyObject());
        return elem;
    }

    std::unique_ptr<DataNode> makeMenuRoot() const
    {
        auto panel = MenuLoaderFixture::makeElement("Panel");
        static_cast<Node*>(panel.get())->setString("id", "menu_panel");
        static_cast<Node*>(panel.get())->setObject("rect", MenuLoaderFixture::makeRect(10.f, 20.f, 640.f, 480.f));

        auto title = MenuLoaderFixture::makeElement("Text");
        static_cast<Node*>(title.get())->setString("id", "menu_title");
        static_cast<Node*>(title.get())->setString("text", "Battle Stars");
        static_cast<Node*>(title.get())->setFloat("fontSize", 36.f);
        static_cast<Node*>(title.get())->setString("font", "assets/fonts/title.ttf");
        static_cast<Node*>(title.get())->setObject("color", MenuLoaderFixture::makeColor(12, 34, 56, 78));

        auto button = MenuLoaderFixture::makeElement("Button");
        static_cast<Node*>(button.get())->setString("id", "start_button");
        static_cast<Node*>(button.get())->setString("text", "Start");
        static_cast<Node*>(button.get())->setFloat("fontSize", 28.f);
        static_cast<Node*>(button.get())->setString("font", "assets/fonts/button.ttf");
        static_cast<Node*>(button.get())->setObject("color", MenuLoaderFixture::makeColor(200, 210, 220, 255));
        static_cast<Node*>(button.get())->setObject("action", MenuLoaderFixture::makeAction("Quit"));

        auto image = MenuLoaderFixture::makeElement("Image");
        static_cast<Node*>(image.get())->setString("id", "logo");
        static_cast<Node*>(image.get())->setString("texture", "assets/ui/logo.png");

        auto root = std::make_unique<Node>();
        root->setString("id", "main_menu");
        std::vector<std::unique_ptr<DataNode>> elements;
        elements.push_back(std::move(panel));
        elements.push_back(std::move(title));
        elements.push_back(std::move(button));
        elements.push_back(std::move(image));
        root->setArray("elements", std::move(elements));
        return root;
    }

    MenuLoaderFixture()
    {
        this->engine = std::make_unique<Engine>(this->window, this->settings);
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

        UIActionFactory::Config uiCfg{ this->engine->events(), this->engine->scenes() };
        this->actionFactory = std::make_shared<UIActionFactory>(uiCfg);
    }

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
    StubPlatformFactory platformFactory;

    EventBus eventBus;
    std::shared_ptr<UIActionFactory> actionFactory;
};

TEST_CASE_METHOD(MenuLoaderFixture, "MenuLoader parses menu elements and forwards path to parser",
    "[unit][menu_loader]"
) {
    Parser fakeParser(this->makeMenuRoot());
    MenuLoader loader(fakeParser, *this->actionFactory);

    const auto data = loader.load("assets/ui/main_menu.json");

    REQUIRE(fakeParser.lastPath == "assets/ui/main_menu.json");
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
    REQUIRE(dynamic_cast<QuitAction *>(button->action.get()) != nullptr);

    int quitCount = 0;
    this->engine->events().subscribe<QuitEvent>([&](const QuitEvent &) { ++quitCount; });
    button->action->execute();
    REQUIRE(quitCount == 1);

    const auto* image = dynamic_cast<ImageElement*>(data.elements[3].get());
    REQUIRE(image != nullptr);
    REQUIRE(image->id == "logo");
    REQUIRE(image->imagePath == "assets/ui/logo.png");
}

TEST_CASE_METHOD(MenuLoaderFixture, "MenuLoader throws on unknown element type",
    "[unit][menu_loader]"
) {
    auto unknown = std::make_unique<Node>();
    unknown->setString("type", "SomethingElse");

    auto root = std::make_unique<Node>();
    root->setString("id", "main_menu");
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(unknown));
    root->setArray("elements", std::move(elements));

    Parser fakeParser(std::move(root));
    MenuLoader loader(fakeParser, *this->actionFactory);

    REQUIRE_THROWS_AS(loader.load("assets/ui/main_menu.json"), std::runtime_error);
}
