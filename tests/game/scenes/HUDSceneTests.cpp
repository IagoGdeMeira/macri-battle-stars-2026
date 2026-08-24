#include "game/scenes/HUDScene.h"

#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubFontFactory.h"
#include "StubRenderer.h"
#include "StubSceneManager.h"
#include "StubTextureFactory.h"

#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/UIIDComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

#include "game/events/PlayerSpawnedEvent.h"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

class HUDSceneFixture
{
public:
    StubRenderer renderer;
    GameSettings settings;
    StubDataParser parser;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    StubSceneManager sceneManager;
    EventBus bus;

    HUDSceneFixture()
    {
        this->parser.registerNode("assets/ui/hud_layout.json", this->makeLayoutNode());
        this->parser.registerNode("assets/ui/widgets/health_bar.json", this->makeHealthBarWidgetNode());
        this->parser.registerNode("assets/ui/widgets/timer.json", this->makeTimerWidgetNode());
        this->parser.registerNode("assets/ui/health_bar_colors.json", this->makeHealthColorsNode());
    }

    std::unique_ptr<StubDataNode> makePos(float x, float y) const
    {
        auto node = std::make_unique<StubDataNode>();
        node->setFloat("x", x);
        node->setFloat("y", y);
        return node;
    }

    std::unique_ptr<StubDataNode> makeSize(float width, float height) const
    {
        auto node = std::make_unique<StubDataNode>();
        node->setFloat("width", width);
        node->setFloat("height", height);
        return node;
    }

    std::unique_ptr<StubDataNode> makeLayoutNode() const
    {
        auto root = std::make_unique<StubDataNode>();

        auto panel = std::make_unique<StubDataNode>();
        panel->setString("type", "panel");
        panel->setObject("position", this->makePos(0.f, 0.f));
        panel->setObject("size", this->makeSize(800.f, 600.f));

        auto flex = std::make_unique<StubDataNode>();
        flex->setString("direction", "Column");
        flex->setString("justify", "FlexStart");
        flex->setString("align", "FlexStart");
        flex->setFloat("gap", 10.f);
        panel->setObject("flex", std::move(flex));

        auto boxModel = std::make_unique<StubDataNode>();
        auto padding = std::make_unique<StubDataNode>();
        padding->setFloat("left", 20.f);
        padding->setFloat("right", 20.f);
        padding->setFloat("top", 10.f);
        padding->setFloat("bottom", 10.f);
        boxModel->setObject("padding", std::move(padding));
        panel->setObject("boxModel", std::move(boxModel));

        auto topBar = std::make_unique<StubDataNode>();
        topBar->setString("type", "panel");
        topBar->setString("id", "topBarContainer");
        topBar->setObject("position", this->makePos(0.f, 0.f));
        topBar->setObject("size", this->makeSize(760.f, 50.f));

        auto topFlex = std::make_unique<StubDataNode>();
        topFlex->setString("direction", "Row");
        topFlex->setString("justify", "SpaceBetween");
        topFlex->setString("align", "Center");
        topFlex->setFloat("gap", 10.f);
        topBar->setObject("flex", std::move(topFlex));

        std::vector<std::unique_ptr<DataNode>> topChildren;

        auto left = std::make_unique<StubDataNode>();
        left->setString("type", "panel");
        left->setString("id", "leftHealthBarContainer");
        left->setObject("position", this->makePos(0.f, 0.f));
        left->setObject("size", this->makeSize(300.f, 30.f));
        auto leftFlex = std::make_unique<StubDataNode>();
        leftFlex->setString("direction", "Row");
        leftFlex->setString("justify", "FlexStart");
        leftFlex->setString("align", "Center");
        left->setObject("flex", std::move(leftFlex));
        topChildren.push_back(std::move(left));

        auto timer = std::make_unique<StubDataNode>();
        timer->setString("type", "widget");
        timer->setString("source", "assets/ui/widgets/timer.json");
        topChildren.push_back(std::move(timer));

        auto right = std::make_unique<StubDataNode>();
        right->setString("type", "panel");
        right->setString("id", "rightHealthBarContainer");
        right->setObject("position", this->makePos(0.f, 0.f));
        right->setObject("size", this->makeSize(300.f, 30.f));
        auto rightFlex = std::make_unique<StubDataNode>();
        rightFlex->setString("direction", "Row");
        rightFlex->setString("justify", "FlexStart");
        rightFlex->setString("align", "Center");
        right->setObject("flex", std::move(rightFlex));
        topChildren.push_back(std::move(right));

        topBar->setArray("children", std::move(topChildren));

        std::vector<std::unique_ptr<DataNode>> panelChildren;
        panelChildren.push_back(std::move(topBar));
        panel->setArray("children", std::move(panelChildren));

        std::vector<std::unique_ptr<DataNode>> elements;
        elements.push_back(std::move(panel));
        root->setArray("elements", std::move(elements));

        return root;
    }

    std::unique_ptr<StubDataNode> makeHealthBarWidgetNode() const
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "widget");
        node->setString("widgetType", "healthBar");
        node->setFloat("width", 300.f);
        node->setFloat("height", 30.f);
        return node;
    }

    std::unique_ptr<StubDataNode> makeTimerWidgetNode() const
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "widget");
        node->setString("widgetType", "timer");
        node->setFloat("initialTime", 99.f);
        node->setString("fontPath", "assets/fonts/yoster-island/yoster.ttf");
        node->setFloat("fontSize", 32.f);
        return node;
    }

    std::unique_ptr<StubDataNode> makeHealthColorsNode() const
    {
        auto root = std::make_unique<StubDataNode>();

        auto makeSegment = [](int r, int g, int b, int sr, int sg, int sb)
        {
            auto seg = std::make_unique<StubDataNode>();
            auto fill = std::make_unique<StubDataNode>();
            fill->setInt("r", r); fill->setInt("g", g); fill->setInt("b", b); fill->setInt("a", 255);
            auto shadow = std::make_unique<StubDataNode>();
            shadow->setInt("r", sr); shadow->setInt("g", sg); shadow->setInt("b", sb); shadow->setInt("a", 255);
            seg->setObject("fill", std::move(fill));
            seg->setObject("shadow", std::move(shadow));
            return seg;
        };

        std::vector<std::unique_ptr<DataNode>> segments;
        segments.push_back(makeSegment(255, 0, 0, 120, 0, 0));
        segments.push_back(makeSegment(255, 255, 0, 120, 120, 0));
        segments.push_back(makeSegment(0, 255, 0, 0, 120, 0));
        root->setArray("segments", std::move(segments));
        return root;
    }

    HUDScene::Config makeConfig()
    {
        HUDScene::Config cfg;
        cfg.eventBus            = &this->bus;
        cfg.sceneManager        = &this->sceneManager;
        cfg.renderer            = &this->renderer;
        cfg.parser              = &this->parser;
        cfg.settings            = &this->settings;
        cfg.fontFactory         = &this->fontFactory;
        cfg.textureFactory      = &this->textureFactory;
        cfg.layoutPath          = "assets/ui/hud_layout.json";
        cfg.healthBarWidgetPath = "assets/ui/widgets/health_bar.json";
        cfg.initialRoundTime    = 99.f;
        return cfg;
    }
};

TEST_CASE_METHOD(HUDSceneFixture, "HUDScene initializes and loads layout", "[integration][hud_scene]")
{
    HUDScene scene(this->makeConfig());
    scene.init();

    auto& comp = scene.world().components();

    bool leftFound = false;
    bool rightFound = false;

    View<UIIDComponent> idView(comp);
    for (auto [entity, id] : idView)
    {
        if (id.id == "leftHealthBarContainer") leftFound = true;
        if (id.id == "rightHealthBarContainer") rightFound = true;
    }

    REQUIRE(leftFound);
    REQUIRE(rightFound);

    View<RoundTimerTag> timerView(comp);
    int timerCount = 0;
    for (auto [entity, tag] : timerView)
    {
        ++timerCount;
        (void)entity;
        (void)tag;
    }
    REQUIRE(timerCount == 1);
}

TEST_CASE_METHOD(HUDSceneFixture, "HUDScene instantiates health bars on PlayerSpawnedEvent", "[integration][hud_scene]")
{
    HUDScene scene(this->makeConfig());
    scene.init();

    auto& comp = scene.world().components();

    Entity leftContainer{0};
    Entity rightContainer{0};

    View<UIIDComponent> idView(comp);
    for (auto [entity, id] : idView)
    {
        if (id.id == "leftHealthBarContainer") leftContainer = entity;
        if (id.id == "rightHealthBarContainer") rightContainer = entity;
    }

    REQUIRE(leftContainer.id != 0);
    REQUIRE(rightContainer.id != 0);

    this->bus.emit<PlayerSpawnedEvent>(PlayerSpawnedEvent{0, "reddie", Entity{0}, 300, 150});
    this->bus.emit<PlayerSpawnedEvent>(PlayerSpawnedEvent{1, "grey", Entity{0}, 200, 100});

    scene.update(0.016f);

    std::map<uint32_t, Entity> barsByPlayer;
    View<HealthBarTag, ParentComponent> barView(comp);
    for (auto [entity, tag, parent] : barView)
    {
        barsByPlayer.insert({tag.playerId, entity});
        (void)parent;
    }

    REQUIRE(barsByPlayer.size() == 2);
    REQUIRE(barsByPlayer.at(0).id != 0);
    REQUIRE(barsByPlayer.at(1).id != 0);

    REQUIRE(comp.get<ParentComponent>(barsByPlayer.at(0)).parent == leftContainer);
    REQUIRE(comp.get<ParentComponent>(barsByPlayer.at(1)).parent == rightContainer);

    View<HealthBarSegmentComponent> segmentView(comp);
    REQUIRE(segmentView.size() >= 4);
}
