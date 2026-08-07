#include "game/include/MapLoader/MapLoader.h"

#include "StubAnimationLoader.h"
#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubRenderer.h"
#include "StubWindow.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/MapComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/TextureLoader/TextureLoader.h"

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/PushboxLoader/PushboxLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

class MapLoaderFixture
{
public:
    World world;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubAnimationLoader animationLoader;
    TextureLoader textureLoader{this->textureFactory};
    EntityFactory entityFactory{{this->world, this->resourceManager, this->textureLoader, this->animationLoader}};
    StubDataParser parser;
    PushboxLoader pushboxLoader{this->parser, this->entityFactory};

    MapLoaderFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<HitboxComponent>();
        comp.registerComponent<HurtboxComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<MapComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<TransformComponent>();
    }

    std::unique_ptr<StubDataNode> makeBackgroundLayer() const
    {
        auto layer = std::make_unique<StubDataNode>();
        layer->setString("texture", "assets/maps/bg.png");
        auto parallax = std::make_unique<StubDataNode>();
        parallax->setFloat("x", 0.5f);
        parallax->setFloat("y", 0.25f);
        layer->setObject("parallaxFactor", std::move(parallax));
        layer->setInt("zIndex", 3);
        layer->setBool("repeatX", true);
        return layer;
    }

    std::unique_ptr<StubDataNode> makeSpawnPoint() const
    {
        auto spawn = std::make_unique<StubDataNode>();
        spawn->setInt("playerId", 2);
        spawn->setFloat("x", 320.f);
        return spawn;
    }

    std::unique_ptr<StubDataNode> makeMapRoot() const
    {
        auto root = std::make_unique<StubDataNode>();
        root->setString("name", "Dojo");
        
        auto floor = std::make_unique<StubDataNode>();
        floor->setFloat("y", 420.f);
        floor->setString("texture", "assets/maps/floor.png");
        floor->setFloat("width", 1280.f);
        floor->setFloat("height", 64.f);
        root->setObject("floor", std::move(floor));

        root->setFloat("gravity", 900.f);
        root->setFloat("floorFriction", 6.5f);
        root->setFloat("airFriction", 2.75f);

        auto bounds = std::make_unique<StubDataNode>();
        bounds->setFloat("left", 0.f);
        bounds->setFloat("right", 1280.f);
        bounds->setFloat("top", -200.f);
        bounds->setFloat("bottom", 500.f);
        root->setObject("worldBounds", std::move(bounds));

        std::vector<std::unique_ptr<DataNode>> backgrounds;
        backgrounds.push_back(this->makeBackgroundLayer());

        auto floorVisual = std::make_unique<StubDataNode>();
        floorVisual->setString("type", "texture");
        floorVisual->setString("texture", "assets/maps/floor.png");
        auto floorParallax = std::make_unique<StubDataNode>();
        floorParallax->setFloat("x", 1.f);
        floorParallax->setFloat("y", 1.f);
        floorVisual->setObject("parallaxFactor", std::move(floorParallax));
        floorVisual->setInt("zIndex", 0);
        backgrounds.push_back(std::move(floorVisual));

        root->setArray("backgroundLayers", std::move(backgrounds));

        std::vector<std::unique_ptr<DataNode>> spawnPoints;
        spawnPoints.push_back(this->makeSpawnPoint());
        root->setArray("spawnPoints", std::move(spawnPoints));

        std::vector<std::unique_ptr<DataNode>> collisions;

        auto floorColl = std::make_unique<StubDataNode>();
        floorColl->setString("shape", "rectangle");
        floorColl->setString("pushboxType", "static");
        auto floorPos = std::make_unique<StubDataNode>();
        floorPos->setFloat("x", 0.f);
        floorPos->setFloat("y", 420.f);
        floorColl->setObject("position", std::move(floorPos));
        auto floorSize = std::make_unique<StubDataNode>();
        floorSize->setFloat("width", 1280.f);
        floorSize->setFloat("height", 64.f);
        floorColl->setObject("size", std::move(floorSize));
        collisions.push_back(std::move(floorColl));

        auto wallColl = std::make_unique<StubDataNode>();
        wallColl->setString("shape", "rectangle");
        wallColl->setString("pushboxType", "static");
        auto wallPos = std::make_unique<StubDataNode>();
        wallPos->setFloat("x", 10.f);
        wallPos->setFloat("y", 20.f);
        wallColl->setObject("position", std::move(wallPos));
        auto wallSize = std::make_unique<StubDataNode>();
        wallSize->setFloat("width", 640.f);
        wallSize->setFloat("height", 48.f);
        wallColl->setObject("size", std::move(wallSize));
        collisions.push_back(std::move(wallColl));

        root->setArray("collisions", std::move(collisions));

        return root;
    }
};

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader parses map data and creates map entity", "[unit][map_loader]")
{
    this->parser.registerNode("assets/maps/dojo.json", this->makeMapRoot());

    MapLoader loader(this->parser, this->entityFactory, this->pushboxLoader);
    Entity mapEntity = loader.load(this->world, "assets/maps/dojo.json");

    auto& comp = this->world.components();

    REQUIRE(comp.has<MapComponent>(mapEntity));
    const auto& map = comp.get<MapComponent>(mapEntity);

    REQUIRE(map.name == "Dojo");
    REQUIRE(map.floorY == 420.f);
    REQUIRE(map.gravity == 900.f);
    REQUIRE(map.floorFriction == 6.5f);
    REQUIRE(map.airFriction == 2.75f);

    REQUIRE(map.spawnPoints.size() == 1);
    REQUIRE(map.spawnPoints[0].playerId == 2u);
    REQUIRE(map.spawnPoints[0].x == 320.f);

    auto parentView = View<ParentComponent>(comp);

    bool foundBackground = false;
    bool foundFloorVisual = false;
    int rectColliderCount = 0;

    for (auto [entity, parent] : parentView)
    {
        if (parent.parent != mapEntity || !comp.has<SpriteComponent>(entity)) continue;

        if (!comp.has<ParallaxComponent>(entity)) continue;
        
        const auto& p = comp.get<ParallaxComponent>(entity);
        if (p.factor.x == Catch::Approx(1.f) && p.factor.y == Catch::Approx(1.f)) foundFloorVisual = true;
        else foundBackground = true; 
    }

    for (auto&& item : View<RectangleColliderComponent>(comp)) (void)item, ++rectColliderCount;

    REQUIRE(rectColliderCount >= 2);
    REQUIRE(foundBackground);
    REQUIRE(foundFloorVisual);
}

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader applies parallax factors from JSON correctly",
    "[unit][map_loader]"
) {
    this->parser.registerNode("assets/maps/dojo.json", this->makeMapRoot());

    MapLoader loader(this->parser, this->entityFactory, this->pushboxLoader);
    Entity mapEntity = loader.load(this->world, "assets/maps/dojo.json");

    auto& comp = this->world.components();

    bool foundCorrectParallax = false;
    for (auto [entity, parent] : View<ParentComponent>(comp))
    {
        if (parent.parent != mapEntity || !comp.has<ParallaxComponent>(entity)) continue;

        const auto& parallax = comp.get<ParallaxComponent>(entity);
        if (parallax.factor.x == Catch::Approx(0.5f) && parallax.factor.y == Catch::Approx(0.25f))
        { foundCorrectParallax = true; break; }
    }

    REQUIRE(foundCorrectParallax);
}
