#include "game/include/HitboxLoader/HitboxLoader.h"

#include "StubAnimationLoader.h"
#include "StubDataParser.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/CircleColliderComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/include/DataParser/DataParser.h"

#include "game/include/EntityFactory/EntityFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>

class HitboxLoaderFixture
{
public:
    HitboxLoaderFixture() :
        textureFactory(),
        textureLoader(this->textureFactory),
        factory(std::make_unique<EntityFactory>(EntityFactory::Config{
            this->world, this->resourceManager, this->textureLoader, this->animLoader})),
        loader(std::make_unique<HitboxLoader>(this->parser, *this->factory))
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<RectangleColliderComponent>();
        comp.registerComponent<CircleColliderComponent>();
        comp.registerComponent<HitboxComponent>();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<HitboxControllerComponent>();
    }

    World world;
    StubDataParser parser;
    StubResourceManager resourceManager;
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader;
    StubAnimationLoader animLoader;
    std::unique_ptr<EntityFactory> factory;
    std::unique_ptr<HitboxLoader> loader;

    std::unique_ptr<StubDataNode> createStateNode(const std::string& name, bool loop)
    {
        auto state = std::make_unique<StubDataNode>();
        state->setString("name", name);
        state->setBool("loop", loop);
        return state;
    }

    std::unique_ptr<StubDataNode> createFrameNode(float duration)
    {
        auto frame = std::make_unique<StubDataNode>();
        frame->setFloat("duration", duration);
        return frame;
    }

    std::unique_ptr<StubDataNode> createHitboxNode(const std::string& type, float x, float y, float width, float height, int damage)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", type);
        node->setFloat("x", x);
        node->setFloat("y", y);
        if (type == "rectangle")
        {
            node->setFloat("width", width);
            node->setFloat("height", height);
        }
        else if (type == "circle") node->setFloat("radius", width);
        node->setInt("damage", damage);
        return node;
    }
};

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader loads a single state with rectangle hitbox", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);
    auto hitbox = this->createHitboxNode("rectangle", 10.f, 5.f, 20.f, 30.f, 5);

    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].duration == 0.1f);
    REQUIRE(controller.frames[0].hitboxes.size() == 1);
    REQUIRE(controller.loop == false);

    Entity hitboxEntity = controller.frames[0].hitboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<HitboxComponent>(hitboxEntity));
    REQUIRE(comp.get<HitboxComponent>(hitboxEntity).damage == 5);
    REQUIRE(comp.has<RectangleColliderComponent>(hitboxEntity));
}

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader loads a single state with circle hitbox", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.15f);
    auto hitbox = this->createHitboxNode("circle", 0.f, 0.f, 15.f, 0.f, 10);

    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].duration == 0.15f);
    REQUIRE(controller.frames[0].hitboxes.size() == 1);

    Entity hitboxEntity = controller.frames[0].hitboxes[0];
    auto& comp = this->world.components();
    REQUIRE(comp.has<HitboxComponent>(hitboxEntity));
    REQUIRE(comp.get<HitboxComponent>(hitboxEntity).damage == 10);
    REQUIRE(comp.has<CircleColliderComponent>(hitboxEntity));
}

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader handles multiple frames in a state", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", true);

    std::vector<std::unique_ptr<DataNode>> frames;
    for (int i = 0; i < 3; ++i)
    {
        auto frame = this->createFrameNode(0.05f);
        std::vector<std::unique_ptr<DataNode>> hitboxes;
        frame->setArray("hitboxes", std::move(hitboxes));
        frames.push_back(std::move(frame));
    }
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 3);
    REQUIRE(controller.loop == true);
    for (const auto& frame : controller.frames) REQUIRE(frame.duration == 0.05f);
}

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader handles multiple states", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    std::vector<std::unique_ptr<DataNode>> states;

    auto idleState = this->createStateNode("Idle", false);
    auto idleFrame = this->createFrameNode(0.2f);
    idleFrame->setArray("hitboxes", {});
    std::vector<std::unique_ptr<DataNode>> idleFrames;
    idleFrames.push_back(std::move(idleFrame));
    idleState->setArray("frames", std::move(idleFrames));
    states.push_back(std::move(idleState));

    auto punchState = this->createStateNode("Punching", false);
    auto punchFrame = this->createFrameNode(0.1f);
    auto hitbox = this->createHitboxNode("rectangle", 10.f, 0.f, 20.f, 20.f, 5);
    std::vector<std::unique_ptr<DataNode>> punchHitboxes;
    punchHitboxes.push_back(std::move(hitbox));
    punchFrame->setArray("hitboxes", std::move(punchHitboxes));
    std::vector<std::unique_ptr<DataNode>> punchFrames;
    punchFrames.push_back(std::move(punchFrame));
    punchState->setArray("frames", std::move(punchFrames));
    states.push_back(std::move(punchState));

    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 2);
    REQUIRE(controllerMap.find(StateId::Idle) != controllerMap.end());
    REQUIRE(controllerMap.find(StateId::Punching) != controllerMap.end());

    const auto& idleController = controllerMap[StateId::Idle];
    REQUIRE(idleController.frames.size() == 1);
    REQUIRE(idleController.frames[0].hitboxes.empty());

    const auto& punchController = controllerMap[StateId::Punching];
    REQUIRE(punchController.frames.size() == 1);
    REQUIRE(punchController.frames[0].hitboxes.size() == 1);
}

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader throws on unknown hitbox type", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.1f);

    auto hitbox = std::make_unique<StubDataNode>();
    hitbox->setString("type", "polygon");
    hitbox->setFloat("x", 0.f);
    hitbox->setFloat("y", 0.f);

    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    REQUIRE_THROWS_AS(this->loader->load(*root, parent, false), std::runtime_error);
}

TEST_CASE_METHOD(HitboxLoaderFixture, "HitboxLoader handles empty hitbox list", "[unit][hitbox_loader]")
{
    auto root = std::make_unique<StubDataNode>();

    auto state = this->createStateNode("Idle", false);
    auto frame = this->createFrameNode(0.2f);
    frame->setArray("hitboxes", {});

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    state->setArray("frames", std::move(frames));

    std::vector<std::unique_ptr<DataNode>> states;
    states.push_back(std::move(state));
    root->setArray("states", std::move(states));

    Entity parent = this->world.entities().create();
    auto controllerMap = this->loader->load(*root, parent, false);

    REQUIRE(controllerMap.size() == 1);
    auto it = controllerMap.find(StateId::Idle);
    REQUIRE(it != controllerMap.end());

    const auto& controller = it->second;
    REQUIRE(controller.frames.size() == 1);
    REQUIRE(controller.frames[0].hitboxes.empty());
}
