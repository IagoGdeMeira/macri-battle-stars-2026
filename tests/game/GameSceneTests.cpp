#include "../../src/game/scenes/GameScene.h"

#include "../../src/domain/components/AirFrictionComponent.h"
#include "../../src/domain/components/AnalogInputComponent.h"
#include "../../src/domain/components/GravityComponent.h"
#include "../../src/domain/components/GroundedComponent.h"
#include "../../src/domain/components/InputBufferComponent.h"
#include "../../src/domain/components/InputComponent.h"
#include "../../src/domain/components/PlayerComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/StateComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/components/VelocityComponent.h"
#include "../../src/domain/include/View/View.h"
#include "../../src/domain/include/StateId/StateId.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/GameSettings/GameSettings.h"
#include "../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/SceneManager/SceneManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../src/engine/include/Window/Window.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Catch::Approx;

class TestNode : public DataNode
{
public:
    void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
    void setInt(const std::string& key, int value) { this->ints[key] = value; }
    void setFloat(const std::string& key, float value) { this->floats[key] = value; }
    void setBool(const std::string& key, bool value) { this->bools[key] = value; }

    void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
    {
        std::vector<TestNode> converted;
        converted.reserve(value.size());
        for (auto& node : value)
        {
            auto* typed = dynamic_cast<TestNode*>(node.get());
            if (!typed) throw std::runtime_error("Unexpected node type");
            converted.push_back(*typed);
        }
        this->arrays[key] = std::move(converted);
    }

    void setObject(const std::string& key, std::unique_ptr<DataNode> value)
    {
        auto* typed = dynamic_cast<TestNode*>(value.get());
        if (!typed) throw std::runtime_error("Unexpected node type");
        this->objects[key] = *typed;
    }

    bool has(const std::string& key) const override
    {
        if (this->strings.contains(key)) return true;
        if (this->ints.contains(key)) return true;
        if (this->floats.contains(key)) return true;
        if (this->bools.contains(key)) return true;
        if (this->arrays.contains(key)) return true;
        if (this->objects.contains(key)) return true;
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

    bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
    {
        auto it = this->bools.find(key);
        return (it != this->bools.end()) ? it->second : fallback;
    }

    std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
    {
        auto it = this->arrays.find(key);
        if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);
        std::vector<std::unique_ptr<DataNode>> out;
        out.reserve(it->second.size());
        for (const auto& node : it->second)  out.push_back(std::make_unique<TestNode>(node));
        return out;
    }

    std::unique_ptr<DataNode> getObject(const std::string& key) const override
    {
        auto it = this->objects.find(key);
        if (it == this->objects.end()) return nullptr;
        return std::make_unique<TestNode>(it->second);
    }

    std::unique_ptr<DataNode> clone() const { return std::make_unique<TestNode>(*this); }

private:
    std::unordered_map<std::string, std::string> strings;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, std::vector<TestNode>> arrays;
    std::unordered_map<std::string, TestNode> objects;
};

class StubTexture : public Texture
{
public:
    int getWidth() const override { return 64; }
    int getHeight() const override { return 96; }
};

class StubRenderer : public Renderer
{
public:
    void clear() override {}
    void present() override {}
    void drawTexture(const DrawTextureCommand&) override {}
    void drawFont(const DrawFontCommand&) override {}
    void drawCircle(const DrawCircleCommand&) override {}
    void drawRectangle(const DrawRectangleCommand&) override {}
    void setViewport(const Viewport&) override {}
};

class StubWindow : public Window
{
public:
    void create(int, int, const char*) override {}
    void setResolution(int, int) override {}
    void setFullscreen(bool) override {}
    void getSize(int& w, int& h) override { w = 800; h = 600; }
};

class StubDataParser : public DataParser
{
public:
    void registerNode(const std::string& path, std::unique_ptr<TestNode> node) { this->nodes[path] = std::move(node); }

    std::unique_ptr<DataNode> parse(const std::string& path) const override
    {
        auto it = this->nodes.find(path);
        if (it != this->nodes.end()) return std::make_unique<TestNode>(*it->second);
        
        throw std::runtime_error("Unexpected parser path: " + path);
    }

private:
    mutable std::unordered_map<std::string, std::unique_ptr<TestNode>> nodes;
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
    std::shared_ptr<Texture> createTexture(const std::string&) override { return std::make_shared<StubTexture>(); }
};

class StubFontFactory : public IFontFactory
{
public:
    std::shared_ptr<Font> createFont(const std::string&) override { return nullptr; }
};

class StubEngine : public Engine
{
public:
    StubEngine(Window& w, GameSettings& s) : Engine(w, s) {}
};

class DummySceneFactory : public SceneFactory
{
public:
    DummySceneFactory() : SceneFactory(SceneFactory::Config{
        .window             = this->dummyWindow,
        .parser             = this->dummyParser,
        .resourceManager    = this->dummyResourceManager,
        .textureLoader      = this->dummyTextureLoader,
        .renderer           = this->dummyRenderer,
        .eventBus           = this->dummyEventBus,
        .settings           = this->dummySettings,
        .engine             = this->dummyEngine,
        .fontFactory        = this->dummyFontFactory,
        .textureFactory     = this->dummyTextureFactory
    }) {}

private:
    StubWindow dummyWindow;
    StubDataParser dummyParser;
    StubResourceManager dummyResourceManager;
    TextureLoader dummyTextureLoader{this->dummyTextureFactory};
    StubRenderer dummyRenderer;
    EventBus dummyEventBus;
    GameSettings dummySettings;
    StubEngine dummyEngine{this->dummyWindow, this->dummySettings};
    StubFontFactory dummyFontFactory;
    StubTextureFactory dummyTextureFactory;
};

class StubSceneManager : public SceneManager
{
public:
    StubSceneManager() : SceneManager(this->dummyFactory, this->dummyEngine) {}
private:
    DummySceneFactory dummyFactory;
    StubEngine dummyEngine{this->dummyWindow, this->dummySettings};
    StubWindow dummyWindow;
    GameSettings dummySettings;
};

class GameSceneFixture
{
public:
    std::unique_ptr<TestNode> makeMapNode()
    {
        auto root = std::make_unique<TestNode>();
        root->setString("name", "TestStage");
        root->setFloat("floor.y", 400.f);
        root->setString("floor.texture", "assets/floor.png");
        root->setFloat("floor.width", 2000.f);
        root->setFloat("floor.height", 50.f);
        auto spawn = std::make_unique<TestNode>();
        spawn->setInt("playerId", 0);
        spawn->setFloat("x", 300.f);
        std::vector<std::unique_ptr<DataNode>> spawns;
        spawns.push_back(std::move(spawn));
        root->setArray("spawnPoints", std::move(spawns));
        root->setArray("walls", {});
        root->setArray("backgroundLayers", {});
        root->setFloat("gravity", 980.f);
        root->setFloat("floorFriction", 5.f);
        root->setFloat("airFriction", 2.f);
        return root;
    }

    std::unique_ptr<TestNode> makeInputBindingsNode()
    {
        auto root = std::make_unique<TestNode>();
        std::vector<std::unique_ptr<DataNode>> players;
        auto player = std::make_unique<TestNode>();
        player->setInt("id", 0);
        player->setArray("bindings", {});
        players.push_back(std::move(player));
        root->setArray("players", std::move(players));
        return root;
    }

    std::unique_ptr<TestNode> makeCombosNode()
    {
        auto root = std::make_unique<TestNode>();
        root->setArray("combos", {});
        return root;
    }

    std::unique_ptr<TestNode> makeTriggersNode()
    {
        auto root = std::make_unique<TestNode>();
        root->setArray("bindings", {});
        return root;
    }

    std::unique_ptr<TestNode> makeCharacterDefNode()
    {
        auto root = std::make_unique<TestNode>();
        root->setString("id", "test_fighter");
        root->setString("texture", "assets/sprites/fighter.png");
        root->setInt("spriteWidth", 64);
        root->setInt("spriteHeight", 96);
        root->setString("animations", "fake_animations.json");
        root->setString("stateMachine", "fake_fsm.json");
        root->setString("collisions", "");
        root->setArray("customStates", {});
        return root;
    }

    std::unique_ptr<TestNode> makeAnimationNode()
    {
        auto root = std::make_unique<TestNode>();
        auto anim = std::make_unique<TestNode>();
        anim->setString("state", "Idle");
        anim->setFloat("frameDuration", 0.1f);
        anim->setBool("loop", true);
        auto frame = std::make_unique<TestNode>();
        frame->setInt("x", 0);
        frame->setInt("y", 0);
        frame->setInt("width", 64);
        frame->setInt("height", 96);
        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frame));
        anim->setArray("frames", std::move(frames));
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(anim));
        root->setArray("animations", std::move(animations));
        return root;
    }

    std::unique_ptr<TestNode> makeFSMNode()
    {
        auto root = std::make_unique<TestNode>();

        auto idleState = std::make_unique<TestNode>();
        idleState->setString("name", "Idle");
        std::vector<std::unique_ptr<DataNode>> states;
        states.push_back(std::move(idleState));
        root->setArray("states", std::move(states));

        auto transition = std::make_unique<TestNode>();
        transition->setString("from", "Idle");
        transition->setString("to", "Idle");
        transition->setString("trigger", "Jumped");
        transition->setArray("conditions", {});

        std::vector<std::unique_ptr<DataNode>> transitions;
        transitions.push_back(std::move(transition));
        root->setArray("transitions", std::move(transitions));

        return root;
    }

    GameSceneFixture()
    {
        this->parser.registerNode("assets/maps/stage1.json", this->makeMapNode());
        this->parser.registerNode("assets/inputs/game_bindings.json", this->makeInputBindingsNode());
        this->parser.registerNode("assets/combos/default_combos.json", this->makeCombosNode());
        this->parser.registerNode("assets/triggers/default_triggers.json", this->makeTriggersNode());
        this->parser.registerNode("assets/characters/knight.json", this->makeCharacterDefNode());
        this->parser.registerNode("fake_animations.json", this->makeAnimationNode());
        this->parser.registerNode("fake_fsm.json", this->makeFSMNode());
    }

    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubFontFactory fontFactory;
    StubDataParser parser;
    TextureLoader textureLoader{this->textureFactory};
    StubEngine engine{this->window, this->settings};
    StubSceneManager sceneManager;
};

TEST_CASE_METHOD(GameSceneFixture, "GameScene initializes and creates player entity",
    "[integration][game_scene]"
) {
    GameScene::Config cfg;
    cfg.eventBus        = &this->engine.events();
    cfg.sceneManager    = &this->sceneManager;
    cfg.renderer        = &this->renderer;
    cfg.window          = &this->window;
    cfg.parser          = &this->parser;
    cfg.resourceManager = &this->resourceManager;
    cfg.textureLoader   = &this->textureLoader;
    cfg.settings        = &this->settings;
    cfg.engine          = &this->engine;
    cfg.fontFactory     = &this->fontFactory;
    cfg.textureFactory  = &this->textureFactory;

    cfg.playerSlots         = { {0, "assets/characters/knight.json"} };
    cfg.mapPath             = "assets/maps/stage1.json";
    cfg.inputBindingsPath   = "assets/inputs/game_bindings.json";
    cfg.combosPath          = "assets/combos/default_combos.json";
    cfg.triggersPath        = "assets/triggers/default_triggers.json";

    GameScene scene(std::move(cfg));
    scene.init();

    auto& world = scene.world();
    auto& components = world.components();

    View<PlayerComponent,
        TransformComponent,
        InputComponent,
        InputBufferComponent,
        VelocityComponent,
        GroundedComponent
    > view(components);

    int count = 0;
    for (auto [entity, player, transform, i_, ib_, v_, g_] : view)
    {
        ++count;
        REQUIRE(player.id == 0);
        REQUIRE(transform.position.x == 300.f);
        REQUIRE(transform.position.y == Approx(400.f - 48.f));
        (void)entity; (void)i_; (void)ib_; (void)v_; (void)g_;
    }
    REQUIRE(count == 1);
}
