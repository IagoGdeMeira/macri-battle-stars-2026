#include "../../src/game/include/AnimationLoader/AnimationLoader.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class AnimationLoaderFixture
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

        bool has(const std::string& key) const override
        {
            return this->strings.contains(key)
                || this->ints.contains(key)
                || this->floats.contains(key)
                || this->bools.contains(key)
                || this->arrays.contains(key);
        }

        std::string getString(const std::string& key) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) throw std::runtime_error("Missing string key: " + key);
            return it->second;
        }

        int getInt(const std::string& key) const override
        {
            const auto it = this->ints.find(key);
            if (it == this->ints.end()) throw std::runtime_error("Missing int key: " + key);
            return it->second;
        }

        float getFloat(const std::string& key) const override
        {
            const auto it = this->floats.find(key);
            if (it == this->floats.end()) throw std::runtime_error("Missing float key: " + key);
            return it->second;
        }

        bool getBool(const std::string& key) const override
        {
            const auto it = this->bools.find(key);
            if (it == this->bools.end()) throw std::runtime_error("Missing bool key: " + key);
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

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<Node>> arrays;
    };

    class Parser : public DataParser
    {
    public:
        explicit Parser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string& filePath) const override
        {
            lastPath = filePath;

            auto* typed = dynamic_cast<Node*>(root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<Node>(*typed);
        }

    private:
        std::unique_ptr<DataNode> root;
    };

    std::unique_ptr<DataNode> makeAnimationsRoot() const
    {
        auto frameA = std::make_unique<Node>();
        frameA->setInt("x", 0);
        frameA->setInt("y", 0);
        frameA->setInt("width", 16);
        frameA->setInt("height", 24);

        auto frameB = std::make_unique<Node>();
        frameB->setInt("x", 16);
        frameB->setInt("y", 0);
        frameB->setInt("width", 16);
        frameB->setInt("height", 24);

        auto idleAnimation = std::make_unique<Node>();
        idleAnimation->setString("state", "Idle");
        idleAnimation->setFloat("frameDuration", 0.08f);
        idleAnimation->setBool("loop", true);

        std::vector<std::unique_ptr<DataNode>> idleFrames;
        idleFrames.push_back(std::move(frameA));
        idleFrames.push_back(std::move(frameB));
        idleAnimation->setArray("frames", std::move(idleFrames));

        auto rootNode = std::make_unique<Node>();
        std::vector<std::unique_ptr<DataNode>> animations;
        animations.push_back(std::move(idleAnimation));
        rootNode->setArray("animations", std::move(animations));

        return rootNode;
    }
};

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader parses mapped states and frames",
    "[unit][animation_loader]"
) {
    Parser parser(this->makeAnimationsRoot());
    AnimationLoader loader(parser);

    const auto animations = loader.load("assets/animations/fighter.json");

    REQUIRE(parser.lastPath == "assets/animations/fighter.json");
    REQUIRE(animations.size() == 1);
    REQUIRE(animations.contains(StateId::Idle));

    const auto& idle = animations.at(StateId::Idle);
    REQUIRE(idle.frameDuration == 0.08f);
    REQUIRE(idle.loop == true);
    REQUIRE(idle.frames.size() == 2);
    REQUIRE(idle.frames[0].x == 0);
    REQUIRE(idle.frames[0].y == 0);
    REQUIRE(idle.frames[0].width == 16);
    REQUIRE(idle.frames[0].height == 24);
    REQUIRE(idle.frames[1].x == 16);
}

TEST_CASE_METHOD(AnimationLoaderFixture, "AnimationLoader rejects unknown state names",
    "[unit][animation_loader]"
) {
    auto frame = std::make_unique<Node>();
    frame->setInt("x", 0);
    frame->setInt("y", 0);
    frame->setInt("width", 16);
    frame->setInt("height", 16);

    auto animation = std::make_unique<Node>();
    animation->setString("state", "NotAState");
    animation->setFloat("frameDuration", 0.1f);
    animation->setBool("loop", true);

    std::vector<std::unique_ptr<DataNode>> frames;
    frames.push_back(std::move(frame));
    animation->setArray("frames", std::move(frames));

    auto rootNode = std::make_unique<Node>();
    std::vector<std::unique_ptr<DataNode>> animations;
    animations.push_back(std::move(animation));
    rootNode->setArray("animations", std::move(animations));

    Parser parser(std::move(rootNode));
    AnimationLoader loader(parser);

    REQUIRE_THROWS(loader.load("assets/animations/bad.json"));
}
