#include "../src/game/include/MenuLoader/MenuLoader.h"

#include "../src/engine/events/QuitEvent.h"
#include "../src/engine/include/DataNode/DataNode.h"
#include "../src/engine/include/DataParser/DataParser.h"
#include "../src/engine/include/EventBus/EventBus.h"
#include "../src/engine/include/ISceneFactory/ISceneFactory.h"
#include "../src/engine/include/Scene/Scene.h"
#include "../src/engine/include/SceneManager/SceneManager.h"

#include "../src/game/ui_actions/QuitAction.h"

#include <catch2/catch_test_macros.hpp>

#include <any>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class MenuLoaderFixture
{
public:
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
            if (!typed) throw std::runtime_error("Unexpected node type in fake tree");
            this->objects[key] = *typed;
        }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<Node> converted;
            converted.reserve(value.size());

            for (auto& node : value)
            {
                auto* typed = dynamic_cast<Node*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type in fake tree");
                converted.push_back(*typed);
            }

            this->arrays[key] = std::move(converted);
        }

        bool has(const std::string& key) const override
        {
            return this->strings.contains(key)
                || this->ints.contains(key)
                || this->floats.contains(key)
                || this->bools.contains(key)
                || this->objects.contains(key)
                || this->arrays.contains(key);
        }

        std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) return fallback;
            return it->second;
        }

        int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
        {
            const auto it = this->ints.find(key);
            if (it == this->ints.end()) return fallback;
            return it->second;
        }

        float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
        {
            const auto it = this->floats.find(key);
            if (it == this->floats.end()) return fallback;
            return it->second;
        }

        bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
        {
            const auto it = this->bools.find(key);
            if (it == this->bools.end()) return fallback;
            return it->second;
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        {
            const auto it = this->arrays.find(key);
            if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);

            std::vector<std::unique_ptr<DataNode>> out;
            out.reserve(it->second.size());

            for (const auto& node : it->second)
            { out.push_back(std::make_unique<Node>(node)); }

            return out;
        }

        std::unique_ptr<DataNode> getObject(const std::string& key) const override
        {
            const auto it = this->objects.find(key);
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

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            this->lastPath = filePath;

            auto* typed = dynamic_cast<Node*>(this->root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    class TestSceneFactory : public ISceneFactory
    {
    public:
        std::unique_ptr<Scene> createScene(SceneId id, std::any data) override
        {
            this->lastId = id;
            this->lastData = std::move(data);
            return std::make_unique<Scene>(this->eventBus);
        }

        EventBus eventBus;
        SceneId lastId { };
        std::any lastData;
    };

    MenuLoaderFixture() : sceneManager(this->sceneFactory), actionFactory({ this->eventBus, this->sceneManager }) {}

    static std::unique_ptr<DataNode> makeEmptyObject()
    {
        return std::make_unique<Node>();
    }

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

    static std::unique_ptr<DataNode> makeAction(const std::string& type)
    {
        auto action = std::make_unique<Node>();
        action->setString("type", type);
        return action;
    }

    static std::unique_ptr<DataNode> makeElement(const std::string& type)
    {
        auto element = std::make_unique<Node>();
        element->setString("type", type);
        element->setObject("rect", makeEmptyObject());
        element->setObject("margin", makeEmptyObject());
        element->setObject("padding", makeEmptyObject());
        element->setObject("border", makeEmptyObject());
        element->setObject("borderRadius", makeEmptyObject());
        return element;
    }

    std::unique_ptr<DataNode> makeMenuRoot() const
    {
        auto panel = makeElement("Panel");
        static_cast<Node*>(panel.get())->setString("id", "menu_panel");
        static_cast<Node*>(panel.get())->setObject("rect", makeRect(10.f, 20.f, 640.f, 480.f));
        static_cast<Node*>(panel.get())->setObject("margin", makeEmptyObject());
        static_cast<Node*>(panel.get())->setObject("padding", makeEmptyObject());
        static_cast<Node*>(panel.get())->setObject("border", makeEmptyObject());
        static_cast<Node*>(panel.get())->setObject("borderRadius", makeEmptyObject());

        auto title = makeElement("Text");
        static_cast<Node*>(title.get())->setString("id", "menu_title");
        static_cast<Node*>(title.get())->setString("text", "Battle Stars");
        static_cast<Node*>(title.get())->setFloat("fontSize", 36.f);
        static_cast<Node*>(title.get())->setString("font", "assets/fonts/title.ttf");
        static_cast<Node*>(title.get())->setObject("color", makeColor(12, 34, 56, 78));

        auto button = makeElement("Button");
        static_cast<Node*>(button.get())->setString("id", "start_button");
        static_cast<Node*>(button.get())->setString("text", "Start");
        static_cast<Node*>(button.get())->setFloat("fontSize", 28.f);
        static_cast<Node*>(button.get())->setString("font", "assets/fonts/button.ttf");
        static_cast<Node*>(button.get())->setObject("color", makeColor(200, 210, 220, 255));
        static_cast<Node*>(button.get())->setObject("action", makeAction("Quit"));

        auto image = makeElement("Image");
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

    TestSceneFactory sceneFactory;
    SceneManager sceneManager;
    EventBus eventBus;
    UIActionFactory actionFactory;
};

TEST_CASE_METHOD(MenuLoaderFixture, "MenuLoader parses menu elements and forwards path to parser",
    "[unit][menu_loader]"
) {
    Parser parser(this->makeMenuRoot());
    MenuLoader loader(parser, this->actionFactory);

    const auto data = loader.load("assets/ui/main_menu.json");

    REQUIRE(parser.lastPath == "assets/ui/main_menu.json");
    REQUIRE(data.id == "main_menu");
    REQUIRE(data.elements.size() == 4);

    const auto* panel = dynamic_cast<PanelElement*>(data.elements[0].get());
    REQUIRE(panel != nullptr);
    REQUIRE(panel->id == "menu_panel");
    REQUIRE(panel->rect.position.x == 10.f);
    REQUIRE(panel->rect.position.y == 20.f);
    REQUIRE(panel->rect.size.width == 640.f);
    REQUIRE(panel->rect.size.height == 480.f);
    REQUIRE(panel->boxModel.margin.left == 0.f);
    REQUIRE(panel->boxModel.padding.bottom == 0.f);

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
    this->eventBus.subscribe<QuitEvent>([&](const QuitEvent&) { ++quitCount; });
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

    Parser parser(std::move(root));
    MenuLoader loader(parser, this->actionFactory);

    REQUIRE_THROWS_AS(loader.load("assets/ui/main_menu.json"), std::runtime_error);
}