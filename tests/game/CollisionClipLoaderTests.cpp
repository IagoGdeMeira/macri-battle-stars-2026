#include "../src/game/include/CollisionClipLoader/CollisionClipLoader.h"
#include "../src/game/include/StateIdMapper/StateIdMapper.h"

#include "../src/engine/include/DataNode/DataNode.h"
#include "../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CollisionClipLoaderFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }
        void setFloat(const std::string& key, float value) { this->floats[key] = value; }
        void setBool(const std::string& key, bool value) { this->bools[key] = value; }

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

        void setObject(const std::string& key, const Node& value) { this->objects[key] = value; }

        bool has(const std::string& key) const override
        {
            return this->strings.contains(key)
                || this->ints.contains(key)
                || this->floats.contains(key)
                || this->bools.contains(key)
                || this->arrays.contains(key)
                || this->objects.contains(key);
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
            if (it == this->objects.end()) throw std::runtime_error("Missing object key: " + key);
            return std::make_unique<Node>(it->second);
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<Node>> arrays;
        std::unordered_map<std::string, Node> objects;
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

    std::unique_ptr<DataNode> makeCollisionClipsRoot() const
    {
        auto hitbox = std::make_unique<Node>();
        hitbox->setFloat("radius", 18.0f);
        {
            Node off;
            off.setFloat("x", 3.0f);
            off.setFloat("y", -2.0f);
            hitbox->setObject("offset", off);
        }
        hitbox->setInt("damage", 25);

        auto hurtbox = std::make_unique<Node>();
        hurtbox->setFloat("width", 36.0f);
        hurtbox->setFloat("height", 22.0f);
        {
            Node off;
            off.setFloat("x", -1.0f);
            off.setFloat("y", 4.0f);
            hurtbox->setObject("offset", off);
        }
        hurtbox->setFloat("damageMultiplier", 1.5f);

        auto frameA = std::make_unique<Node>();
        frameA->setFloat("duration", 0.12f);

        std::vector<std::unique_ptr<DataNode>> hitboxesA;
        hitboxesA.push_back(std::move(hitbox));
        frameA->setArray("hitboxes", std::move(hitboxesA));

        std::vector<std::unique_ptr<DataNode>> hurtboxesA;
        hurtboxesA.push_back(std::move(hurtbox));
        frameA->setArray("hurtboxes", std::move(hurtboxesA));

        auto frameB = std::make_unique<Node>();
        frameB->setFloat("duration", 0.08f);

        auto clip = std::make_unique<Node>();
        clip->setString("state", "Idle");
        clip->setBool("loop", true);

        std::vector<std::unique_ptr<DataNode>> frames;
        frames.push_back(std::move(frameA));
        frames.push_back(std::move(frameB));
        clip->setArray("frames", std::move(frames));

        auto rootNode = std::make_unique<Node>();
        std::vector<std::unique_ptr<DataNode>> clips;
        clips.push_back(std::move(clip));
        rootNode->setArray("collisionClips", std::move(clips));

        return rootNode;
    }
};

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses clips, frames and colliders",
    "[unit][collision_clip_loader]"
) {
    Parser parser(this->makeCollisionClipsRoot());
    CollisionClipLoader loader(parser);

    const auto clips = loader.load("assets/collision/fighter.json");

    REQUIRE(parser.lastPath == "assets/collision/fighter.json");
    REQUIRE(clips.size() == 1);
    REQUIRE(clips.contains(StateId::Idle));

    const auto& idle = clips.at(StateId::Idle);
    REQUIRE(idle.loop == true);
    REQUIRE(idle.frames.size() == 2);

    REQUIRE(idle.frames[0].duration == 0.12f);
    REQUIRE(idle.frames[0].hitboxes.size() == 1);
    REQUIRE(idle.frames[0].hurtboxes.size() == 1);

    const auto& hitbox = idle.frames[0].hitboxes[0];
    REQUIRE(hitbox.damage == 25);
    REQUIRE(hitbox.collider != nullptr);
    REQUIRE(hitbox.collider->getType() == ColliderDef::ColliderType::Circle);

    auto* circle = dynamic_cast<CircleDef*>(hitbox.collider.get());
    REQUIRE(circle != nullptr);
    REQUIRE(circle->radius == 18.0f);
    REQUIRE(circle->offset.x == 3.0f);
    REQUIRE(circle->offset.y == -2.0f);

    const auto& hurtbox = idle.frames[0].hurtboxes[0];
    REQUIRE(hurtbox.damageMultiplier == 1.5f);
    REQUIRE(hurtbox.collider != nullptr);
    REQUIRE(hurtbox.collider->getType() == ColliderDef::ColliderType::Rectangle);

    auto* rect = dynamic_cast<RectangleDef*>(hurtbox.collider.get());
    REQUIRE(rect != nullptr);
    REQUIRE(rect->width == 36.0f);
    REQUIRE(rect->height == 22.0f);
    REQUIRE(rect->offset.x == -1.0f);
    REQUIRE(rect->offset.y == 4.0f);

    REQUIRE(idle.frames[1].duration == 0.08f);
    REQUIRE(idle.frames[1].hitboxes.empty());
    REQUIRE(idle.frames[1].hurtboxes.empty());
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader applies default fields when omitted",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<Node>();
    hitbox->setFloat("radius", 5.0f);

    auto hurtbox = std::make_unique<Node>();
    hurtbox->setFloat("width", 12.0f);
    hurtbox->setFloat("height", 6.0f);

    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.2f);

    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    std::vector<std::unique_ptr<DataNode>> hurtboxes;
    hurtboxes.push_back(std::move(hurtbox));
    frame->setArray("hurtboxes", std::move(hurtboxes));

    auto clip = std::make_unique<Node>();
    clip->setString("state", "Idle");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    const auto clipsMap = loader.load("assets/collision/defaults.json");

    REQUIRE(clipsMap.at(StateId::Idle).loop == false);

    const auto& loadedFrame = clipsMap.at(StateId::Idle).frames[0];
    REQUIRE(loadedFrame.hitboxes[0].damage == 0);
    REQUIRE(loadedFrame.hurtboxes[0].damageMultiplier == 1.0f);

    auto* loadedCircle = dynamic_cast<CircleDef*>(loadedFrame.hitboxes[0].collider.get());
    REQUIRE(loadedCircle != nullptr);
    REQUIRE(loadedCircle->offset.x == 0.0f);
    REQUIRE(loadedCircle->offset.y == 0.0f);

    auto* loadedRect = dynamic_cast<RectangleDef*>(loadedFrame.hurtboxes[0].collider.get());
    REQUIRE(loadedRect != nullptr);
    REQUIRE(loadedRect->offset.x == 0.0f);
    REQUIRE(loadedRect->offset.y == 0.0f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader rejects unknown state names",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<Node>();
    clip->setString("state", "NotAState");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    REQUIRE_THROWS_AS(loader.load("assets/collision/invalid_state.json"), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader resolves custom state names with mapper",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<Node>();
    clip->setString("state", "PowerCharge");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    StateIdMapper mapper;
    const auto customState = mapper.addCustomMapping("PowerCharge");

    const auto loadedClips = loader.load("assets/collision/custom.json", mapper);

    REQUIRE(loadedClips.size() == 1);
    REQUIRE(loadedClips.contains(customState));
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader with mapper rejects unmapped custom state",
    "[unit][collision_clip_loader]"
) {
    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.1f);

    auto clip = std::make_unique<Node>();
    clip->setString("state", "PowerCharge");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    StateIdMapper mapper;

    REQUIRE_THROWS_AS(loader.load("assets/collision/custom_bad.json", mapper), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader rejects colliders without required shape fields",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<Node>();
    hitbox->setInt("damage", 10);

    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.1f);

    std::vector<std::unique_ptr<DataNode>> hitboxes;
    hitboxes.push_back(std::move(hitbox));
    frame->setArray("hitboxes", std::move(hitboxes));

    auto clip = std::make_unique<Node>();
    clip->setString("state", "Idle");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    REQUIRE_THROWS_AS(loader.load("assets/collision/bad_collider.json"), std::runtime_error);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses pushboxes with correct properties",
    "[unit][collision_clip_loader]"
) {
    auto pushbox = std::make_unique<Node>();
    pushbox->setFloat("radius", 12.0f);
    {
        Node off;
        off.setFloat("x", 2.0f);
        off.setFloat("y", -1.0f);
        pushbox->setObject("offset", off);
    }
    pushbox->setFloat("mass", 0.5f);
    pushbox->setFloat("pushResistance", 2.0f);
    pushbox->setString("type", "Static");

    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.15f);

    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<Node>();
    clip->setString("state", "Idle");
    clip->setBool("loop", false);

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    const auto clips_map = loader.load("assets/collision/with_pushbox.json");

    REQUIRE(clips_map.size() == 1);
    REQUIRE(clips_map.contains(StateId::Idle));

    const auto& idle = clips_map.at(StateId::Idle);
    REQUIRE(idle.frames.size() == 1);
    REQUIRE(idle.frames[0].pushboxes.size() == 1);

    const auto& pb = idle.frames[0].pushboxes[0];
    REQUIRE(pb.mass == 0.5f);
    REQUIRE(pb.pushResistance == 2.0f);
    REQUIRE(pb.type == PushboxDef::Type::Static);
    REQUIRE(pb.collider != nullptr);

    auto* circle = dynamic_cast<CircleDef*>(pb.collider.get());
    REQUIRE(circle != nullptr);
    REQUIRE(circle->radius == 12.0f);
    REQUIRE(circle->offset.x == 2.0f);
    REQUIRE(circle->offset.y == -1.0f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses dynamic pushboxes by default",
    "[unit][collision_clip_loader]"
) {
    auto pushbox = std::make_unique<Node>();
    pushbox->setFloat("radius", 10.0f);

    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.1f);

    std::vector<std::unique_ptr<DataNode>> pushboxes;
    pushboxes.push_back(std::move(pushbox));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<Node>();
    clip->setString("state", "Idle");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    const auto clips_map = loader.load("assets/collision/dynamic_default.json");

    const auto& idle = clips_map.at(StateId::Idle);
    const auto& pb = idle.frames[0].pushboxes[0];

    REQUIRE(pb.type == PushboxDef::Type::Dynamic);
    REQUIRE(pb.mass == 1.0f);
    REQUIRE(pb.pushResistance == 1.0f);
}

TEST_CASE_METHOD(CollisionClipLoaderFixture, "CollisionClipLoader parses mixed colliders in same frame",
    "[unit][collision_clip_loader]"
) {
    auto hitbox = std::make_unique<Node>();
    hitbox->setFloat("radius", 5.0f);
    hitbox->setInt("damage", 15);

    auto hurtbox = std::make_unique<Node>();
    hurtbox->setFloat("width", 20.0f);
    hurtbox->setFloat("height", 15.0f);
    hurtbox->setFloat("damageMultiplier", 0.8f);

    auto pushbox = std::make_unique<Node>();
    pushbox->setFloat("radius", 25.0f);
    pushbox->setFloat("mass", 1.5f);

    auto frame = std::make_unique<Node>();
    frame->setFloat("duration", 0.2f);

    std::vector<std::unique_ptr<DataNode>> hitboxes, hurtboxes, pushboxes;
    hitboxes.push_back(std::move(hitbox));
    hurtboxes.push_back(std::move(hurtbox));
    pushboxes.push_back(std::move(pushbox));

    frame->setArray("hitboxes", std::move(hitboxes));
    frame->setArray("hurtboxes", std::move(hurtboxes));
    frame->setArray("pushboxes", std::move(pushboxes));

    auto clip = std::make_unique<Node>();
    clip->setString("state", "Idle");

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    clip->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> clips;
    clips.push_back(std::move(clip));
    rootNode->setArray("collisionClips", std::move(clips));

    Parser parser(std::move(rootNode));
    CollisionClipLoader loader(parser);

    const auto clips_map = loader.load("assets/collision/mixed.json");

    const auto& idle = clips_map.at(StateId::Idle);
    const auto& frame_data = idle.frames[0];

    REQUIRE(frame_data.hitboxes.size() == 1);
    REQUIRE(frame_data.hurtboxes.size() == 1);
    REQUIRE(frame_data.pushboxes.size() == 1);

    REQUIRE(frame_data.hitboxes[0].damage == 15);
    REQUIRE(frame_data.hurtboxes[0].damageMultiplier == 0.8f);
    REQUIRE(frame_data.pushboxes[0].mass == 1.5f);
}
