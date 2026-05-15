#include "../../src/engine/include/SceneFactory/SceneFactory.h"

#include "../../src/domain/include/Color/Color.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"
#include "../../src/engine/include/Engine/Engine.h"
#include "../../src/engine/include/InputContext/InputContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"
#include "../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../src/engine/include/Window/Window.h"

#include "../../src/game/include/AnimationLoader/AnimationLoader.h"
#include "../../src/game/include/Camera2D/Camera2D.h"
#include "../../src/game/include/CharacterDefinitionLoader/CharacterDefinitionLoader.h"
#include "../../src/game/include/CharacterLoader/CharacterLoader.h"
#include "../../src/game/include/CharacterRoster/CharacterRoster.h"
#include "../../src/game/include/CollisionClipLoader/CollisionClipLoader.h"
#include "../../src/game/include/Combo/Combo.h"
#include "../../src/game/include/MapRoster/MapRoster.h"
#include "../../src/game/include/StateMachineLoader/StateMachineLoader.h"
#include "../../src/game/include/TriggerContext/TriggerContext.h"
#include "../../src/game/scenes/GameScene/GameScene.h"
#include "../../src/game/scenes/SelectionScene/SelectionScene.h"
#include "../../src/game/scenes/TitleScene/TitleScene.h"

#include <any>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class SceneFactoryFixture
{
public:
    class DummyNode : public DataNode
    {
    public:
        bool has(const std::string&) const override { return false; }
        std::string getString(const std::string&, const std::string& fallback = DataNode::defaultStringFallback) const override { return fallback; }
        int getInt(const std::string&, const int& fallback = DataNode::defaultIntFallback) const override { return fallback; }
        float getFloat(const std::string&, const float& fallback = DataNode::defaultFloatFallback) const override { return fallback; }
        bool getBool(const std::string&, const bool& fallback = DataNode::defaultBoolFallback) const override { return fallback; }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string&) const override
        { return {}; }
    };

    class DummyParser : public DataParser
    {
    public:
        std::unique_ptr<DataNode> parse(const std::string&) const override
        { return std::make_unique<DummyNode>(); }
    };

    class StubTexture : public Texture
    {
    public:
        int getWidth() const override { return 0; }
        int getHeight() const override { return 0; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const Texture&, const Renderer::DrawTextureParams&) override {}
        void drawText(const Font&, std::string, const Renderer::DrawTextParams&) override {}
        void drawRectOutline(const Rectangle&, const Color&) override {}
        void drawRectFilled(const Rectangle&, const Color&) override {}
        void drawCircleOutline(const Circle&, const Color&) override {}
        void drawCircleFilled(const Circle&, const Color&) override {}
        void setViewport(const Viewport&) override {}
    };

    class StubFactory : public ITextureFactory
    {
    public:
        std::shared_ptr<Texture> createTexture(const std::string&) override
        { return std::make_shared<StubTexture>(); }
    };

    class StubWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}

        void getSize(int& width, int& height) override
        {
            width = 800;
            height = 600;
        }
    };

    SceneFactoryFixture() :
        resourceManager(threadPool),
        textureLoader(this->factory),
        characterLoader({
            .defLoader = this->definitionLoader,
            .animLoader = this->animationLoader,
            .fsmLoader = this->machineLoader,
            .resourceManager = this->resourceManager,
            .textureLoader = this->textureLoader,
            .clipLoader = this->clipLoader
        }),
        sceneFactory({
            this->window,
            this->inputContext,
            this->triggerContext,
            this->roster,
            this->characterLoader,
            this->camera,
            this->combos,
            this->mapRoster,
            this->parser,
            this->resourceManager,
            this->textureLoader
        }),
        engine(this->window, this->sceneFactory)
    {
        this->sceneFactory.engine = &this->engine;
        this->roster.addEntry(CharacterEntry{ "fighter_01", "Fighter 01", "assets/characters/fighter_01.json" });
        this->mapRoster.addEntry(MapEntry{ "default", "Default", "assets/maps/default.json" });
    }

    void enableRenderer() { this->engine.setRenderer(this->renderer); }

    StubWindow window;
    InputContext inputContext;
    TriggerContext triggerContext;
    CharacterRoster roster;
    MapRoster mapRoster;
    Camera2D camera;
    std::vector<Combo> combos;

    ThreadPool threadPool{ 1 };
    ResourceManager resourceManager;
    StubRenderer renderer;
    StubFactory factory;

    DummyParser definitionParser;
    DummyParser animationParser;
    DummyParser machineParser;
    DummyParser clipParser;
    DummyParser parser;

    CharacterDefinitionLoader definitionLoader{ this->definitionParser };
    AnimationLoader animationLoader{ this->animationParser };
    StateMachineLoader machineLoader{ this->machineParser };
    CollisionClipLoader clipLoader{ this->clipParser };
    TextureLoader textureLoader;
    CharacterLoader characterLoader;

    SceneFactory sceneFactory;
    Engine engine;
};

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory throws when engine is not bound",
    "[unit][scene_factory]"
) {
    this->sceneFactory.engine = nullptr;

    REQUIRE_THROWS(this->sceneFactory.createScene(SceneId::Title, std::monostate{}));
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory throws when renderer is not configured in Engine",
    "[unit][scene_factory]"
) { REQUIRE_THROWS(this->sceneFactory.createScene(SceneId::Title, std::monostate{})); }

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory creates TitleScene when dependencies are configured",
    "[unit][scene_factory]"
) {
    this->enableRenderer();

    auto scene = this->sceneFactory.createScene(SceneId::Title, std::monostate{});

    REQUIRE(dynamic_cast<TitleScene*>(scene.get()) != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory creates SelectionScene when dependencies are configured",
    "[unit][scene_factory]"
) {
    this->enableRenderer();

    auto scene = this->sceneFactory.createScene(SceneId::Selection, std::monostate{});

    REQUIRE(dynamic_cast<SelectionScene*>(scene.get()) != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory creates GameScene when provided valid player slots",
    "[unit][scene_factory]"
) {
    this->enableRenderer();

    std::vector<GameScene::PlayerSlot> slots;
    slots.push_back(GameScene::PlayerSlot{ 1, "assets/characters/fighter_01.json" });

    auto scene = this->sceneFactory.createScene(SceneId::Game, std::move(slots));

    REQUIRE(dynamic_cast<GameScene*>(scene.get()) != nullptr);
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory throws for invalid payload in Game scene",
    "[unit][scene_factory]"
) {
    this->enableRenderer();

    REQUIRE_THROWS(this->sceneFactory.createScene(SceneId::Game, std::monostate{}));
}

TEST_CASE_METHOD(SceneFactoryFixture, "SceneFactory throws for unknown SceneId",
    "[unit][scene_factory]"
) {
    this->enableRenderer();

    REQUIRE_THROWS(this->sceneFactory.createScene(static_cast<SceneId>(999), std::monostate{}));
}
