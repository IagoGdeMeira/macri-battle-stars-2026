#include "../../src/game/include/MapLoader/MapLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"
#include "../stubs/StubResourceManager.h"
#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubRenderer.h"
#include "../stubs/StubWindow.h"

#include "../../src/domain/components/CircleColliderComponent.h"
#include "../../src/domain/components/HitboxComponent.h"
#include "../../src/domain/components/HurtboxComponent.h"
#include "../../src/domain/components/LocalTransform.h"
#include "../../src/domain/components/MapComponent.h"
#include "../../src/domain/components/ParallaxComponent.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/PushboxComponent.h"
#include "../../src/domain/components/RectangleColliderComponent.h"
#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/ShapeRenderComponent.h"
#include "../../src/domain/components/SpriteComponent.h"
#include "../../src/domain/components/TransformComponent.h"
#include "../../src/domain/include/ComponentManager/ComponentManager.h"
#include "../../src/domain/include/View/View.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/TextureLoader/TextureLoader.h"

#include "../../src/game/include/EntityFactory/EntityFactory.h"

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
    TextureLoader textureLoader{this->textureFactory};
    EntityFactory entityFactory{this->world, this->resourceManager, this->textureLoader};
    StubDataParser parser;

    MapLoaderFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<MapComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<ShapeRenderComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<PushboxComponent>();
        comp.registerComponent<HitboxComponent>();
        comp.registerComponent<HurtboxComponent>();
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

    std::unique_ptr<StubDataNode> makeWall() const
    {
        auto wall = std::make_unique<StubDataNode>();
        wall->setFloat("x", 10.f);
        wall->setFloat("y", 20.f);
        wall->setFloat("width", 640.f);
        wall->setFloat("height", 48.f);
        return wall;
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
        root->setArray("backgroundLayers", std::move(backgrounds));

        std::vector<std::unique_ptr<DataNode>> walls;
        walls.push_back(this->makeWall());
        root->setArray("walls", std::move(walls));

        std::vector<std::unique_ptr<DataNode>> spawnPoints;
        spawnPoints.push_back(this->makeSpawnPoint());
        root->setArray("spawnPoints", std::move(spawnPoints));

        return root;
    }
};

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader parses map data and creates map entity",
    "[unit][map_loader]"
) {
    this->parser.registerNode("assets/maps/dojo.json", this->makeMapRoot());

    MapLoader loader(this->parser, this->entityFactory);
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
    bool foundFloor = false;
    int rectColliderCount = 0;

    for (auto [entity, parent] : parentView)
    {
        if (parent.parent != mapEntity || !comp.has<SpriteComponent>(entity)) continue;

        if (comp.has<ParallaxComponent>(entity)) foundBackground = true;
        if (comp.has<RectangleColliderComponent>(entity)) foundFloor = true;
    }

    for (auto [entity, collider] : View<RectangleColliderComponent>(comp)) ++rectColliderCount;
    
    REQUIRE(rectColliderCount >= 2);
    REQUIRE(foundBackground);
    REQUIRE(foundFloor);
}

TEST_CASE_METHOD(MapLoaderFixture, "MapLoader applies parallax factors from JSON correctly",
    "[unit][map_loader]"
) {
    this->parser.registerNode("assets/maps/dojo.json", this->makeMapRoot());

    MapLoader loader(this->parser, this->entityFactory);
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
