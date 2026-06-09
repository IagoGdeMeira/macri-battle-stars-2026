#include "../../src/game/include/CollisionClipLoader/CollisionClipLoader.h"

#include "../stubs/StubDataNode.h"
#include "../stubs/StubDataParser.h"

#include "../../src/game/include/StateIdMapper/StateIdMapper.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class CollisionClipLoaderFixture
{
public:
    std::unique_ptr<StubDataNode> makeOffset(float x, float y) const
    {
        auto off = std::make_unique<StubDataNode>();
        off->setFloat("x", x);
        off->setFloat("y", y);
        return off;
    }
};

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses clips, frames and colliders",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<StubDataNode>();
    hitbox->setFloat("radius", 18.f);
    hitbox->setObject("offset", this->makeOffset(3.f, -2.f));
    hitbox->setInt("damage", 25);

    auto hurtbox = std::make_unique<StubDataNode>();
    hurtbox->setFloat("width", 36.f);
    hurtbox->setFloat("height", 22.f);
    hurtbox->setObject("offset", this->makeOffset(-1.f, 4.f));
    hurtbox->setFloat("damageMultiplier", 1.5f);

    auto frameA = std::make_unique<StubDataNode>();
    frameA->setFloat("duration", 0.12f);
    std::vector<std::unique_ptr<DataNode>> hitboxesA;
    hitboxesA.push_back(std::move(hitbox));
    frameA->setArray("hitboxes", std::move(hitboxesA));
    std::vector<std::unique_ptr<DataNode>> hurtboxesA;
    hurtboxesA.push_back(std::move(hurtbox));
    frameA->setArray("hurtboxes", std::move(hurtboxesA));

    auto frameB = std::make_unique<StubDataNode>();
    frameB->setFloat("duration", 0.08f);

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    clip->setBool("loop", true);
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frameA));
    frames.push_back(std::move(frameB));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("assets/collision/fighter.json", std::move(rootNode));

    CollisionClipLoader loader(parser);
    const auto clipsMap = loader.load("assets/collision/fighter.json");

    REQUIRE(clipsMap.size() == 1);
    REQUIRE(clipsMap.contains(StateId::Idle));

    const auto& idle = clipsMap.at(StateId::Idle);
    REQUIRE(idle.loop == true);
    REQUIRE(idle.frames.size() == 2);

    const auto& frame0 = idle.frames[0];
    REQUIRE(frame0.duration == 0.12f);
    REQUIRE(frame0.hitboxes.size() == 1);
    REQUIRE(frame0.hurtboxes.size() == 1);
    REQUIRE(frame0.pushboxes.empty());

    const auto& hb = frame0.hitboxes[0];
    REQUIRE(hb.damage == 25);
    REQUIRE(hb.collider->getType() == ColliderDef::ColliderType::Circle);
    auto* circle = dynamic_cast<CircleDef*>(hb.collider.get());
    REQUIRE(circle != nullptr);
    REQUIRE(circle->radius == 18.f);
    REQUIRE(circle->offset.x == 3.f);
    REQUIRE(circle->offset.y == -2.f);

    const auto& hurt = frame0.hurtboxes[0];
    REQUIRE(hurt.damageMultiplier == 1.5f);
    REQUIRE(hurt.collider->getType() == ColliderDef::ColliderType::Rectangle);
    auto* rect = dynamic_cast<RectangleDef*>(hurt.collider.get());
    REQUIRE(rect != nullptr);
    REQUIRE(rect->width == 36.f);
    REQUIRE(rect->height == 22.f);
    REQUIRE(rect->offset.x == -1.f);
    REQUIRE(rect->offset.y == 4.f);

    const auto& frame1 = idle.frames[1];
    REQUIRE(frame1.duration == 0.08f);
    REQUIRE(frame1.hitboxes.empty());
    REQUIRE(frame1.hurtboxes.empty());
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader applies default fields when omitted",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<StubDataNode>();
    hitbox->setFloat("radius", 5.f);

    auto hurtbox = std::make_unique<StubDataNode>();
    hurtbox->setFloat("width", 12.f);
    hurtbox->setFloat("height", 6.f);

    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.2f);
    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));
    std::vector<std::unique_ptr<DataNode>> hurtboxes;
    hurtboxes.push_back(std::move(hurtbox));
    frame->setArray("hurtboxes", std::move(hurtboxes));

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("test.json", std::move(root));
    CollisionClipLoader loader(parser);
    const auto clipsMap = loader.load("test.json");

    REQUIRE(clipsMap.at(StateId::Idle).loop == false);

    const auto& loadedFrame = clipsMap.at(StateId::Idle).frames[0];
    REQUIRE(loadedFrame.hitboxes[0].damage == 0);
    REQUIRE(loadedFrame.hurtboxes[0].damageMultiplier == 1.f);

    auto* circle = dynamic_cast<CircleDef*>(loadedFrame.hitboxes[0].collider.get());
    REQUIRE(circle != nullptr);
    REQUIRE(circle->offset.x == 0.f);
    REQUIRE(circle->offset.y == 0.f);

    auto* rect = dynamic_cast<RectangleDef*>(loadedFrame.hurtboxes[0].collider.get());
    REQUIRE(rect != nullptr);
    REQUIRE(rect->offset.x == 0.f);
    REQUIRE(rect->offset.y == 0.f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader rejects unknown state names",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "NotAState");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("bad.json", std::move(root));
    CollisionClipLoader loader(parser);
    REQUIRE_THROWS_AS(loader.load("bad.json"), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader resolves custom state names with mapper",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "PowerCharge");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("custom.json", std::move(root));

    StateIdMapper mapper;
    const auto customState = mapper.addCustomMapping("PowerCharge");

    CollisionClipLoader loader(parser);
    const auto loaded = loader.load("custom.json", mapper);
    REQUIRE(loaded.size() == 1);
    REQUIRE(loaded.contains(customState));
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader with mapper rejects unmapped custom state",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "PowerCharge");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("bad_custom.json", std::move(root));

    StateIdMapper mapper;

    CollisionClipLoader loader(parser);
    REQUIRE_THROWS_AS(loader.load("bad_custom.json", mapper), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader rejects colliders without required shape fields",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<StubDataNode>();
    hitbox->setInt("damage", 10);

    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.1f);
    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("bad_collider.json", std::move(root));
    CollisionClipLoader loader(parser);
    REQUIRE_THROWS_AS(loader.load("bad_collider.json"), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses pushboxes with correct properties",
    "[unit][collision_clip_loader]"
) {
    auto pushbox = std::make_unique<StubDataNode>();
    pushbox->setFloat("radius", 12.f);
    pushbox->setObject("offset", this->makeOffset(2.f, -1.f));
    pushbox->setFloat("mass", 0.5f);
    pushbox->setFloat("pushResistance", 2.f);
    pushbox->setString("type", "Static");

    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.15f);
    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    clip->setBool("loop", false);
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("pushbox.json", std::move(root));
    CollisionClipLoader loader(parser);
    const auto clipsMap = loader.load("pushbox.json");

    const auto& idle = clipsMap.at(StateId::Idle);
    REQUIRE(idle.frames[0].pushboxes.size() == 1);
    const auto& pb = idle.frames[0].pushboxes[0];
    REQUIRE(pb.mass == 0.5f);
    REQUIRE(pb.pushResistance == 2.f);
    REQUIRE(pb.type == PushboxDef::Type::Static);
    auto* circle = dynamic_cast<CircleDef*>(pb.collider.get());
    REQUIRE(circle != nullptr);
    REQUIRE(circle->radius == 12.f);
    REQUIRE(circle->offset.x == 2.f);
    REQUIRE(circle->offset.y == -1.f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses dynamic pushboxes by default",
    "[unit][collision_clip_loader]"
) {
    auto pushbox = std::make_unique<StubDataNode>();
    pushbox->setFloat("radius", 10.f);

    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.1f);
    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("dynamic_default.json", std::move(root));
    CollisionClipLoader loader(parser);
    const auto clipsMap = loader.load("dynamic_default.json");
    const auto& pb = clipsMap.at(StateId::Idle).frames[0].pushboxes[0];
    REQUIRE(pb.type == PushboxDef::Type::Dynamic);
    REQUIRE(pb.mass == 1.f);
    REQUIRE(pb.pushResistance == 1.f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses mixed colliders in same frame",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<StubDataNode>();
    hitbox->setFloat("radius", 5.f);
    hitbox->setInt("damage", 15);

    auto hurtbox = std::make_unique<StubDataNode>();
    hurtbox->setFloat("width", 20.f);
    hurtbox->setFloat("height", 15.f);
    hurtbox->setFloat("damageMultiplier", 0.8f);

    auto pushbox = std::make_unique<StubDataNode>();
    pushbox->setFloat("radius", 25.f);
    pushbox->setFloat("mass", 1.5f);

    auto frame = std::make_unique<StubDataNode>();
    frame->setFloat("duration", 0.2f);
    std::vector<std::unique_ptr<DataNode>> hitboxes, hurtboxes, pushboxes;
    hitboxes.push_back(std::move(hitbox));
    hurtboxes.push_back(std::move(hurtbox));
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("hitboxes", std::move(hitboxes));
    frame->setArray("hurtboxes", std::move(hurtboxes));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<StubDataNode>();
    clip->setString("state", "Idle");
    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto root = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    root->setArray("collisionClips", std::move(clips));

    StubDataParser parser;
    parser.registerNode("mixed.json", std::move(root));
    CollisionClipLoader loader(parser);
    const auto clipsMap = loader.load("mixed.json");
    const auto& frame_data = clipsMap.at(StateId::Idle).frames[0];
    REQUIRE(frame_data.hitboxes.size() == 1);
    REQUIRE(frame_data.hurtboxes.size() == 1);
    REQUIRE(frame_data.pushboxes.size() == 1);
    REQUIRE(frame_data.hitboxes[0].damage == 15);
    REQUIRE(frame_data.hurtboxes[0].damageMultiplier == 0.8f);
    REQUIRE(frame_data.pushboxes[0].mass == 1.5f);
}
