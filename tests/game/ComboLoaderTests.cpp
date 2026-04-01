#include "../../src/game/include/ComboLoader/ComboLoader.h"

#include "../../src/engine/include/DataNode/DataNode.h"
#include "../../src/engine/include/DataParser/DataParser.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Stub
{
public:
    class StubNode : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value) { this->strings[key] = value; }
        void setInt(const std::string& key, int value) { this->ints[key] = value; }
        void setFloat(const std::string& key, float value) { this->floats[key] = value; }
        void setBool(const std::string& key, bool value) { this->bools[key] = value; }

        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
        {
            std::vector<StubNode> converted;
            converted.reserve(value.size());

            for (auto& node : value)
            {
                auto* typed = dynamic_cast<StubNode*>(node.get());
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
            { out.push_back(std::make_unique<StubNode>(node)); }

            return out;
        }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::vector<StubNode>> arrays;
    };

    class StubParser : public DataParser
    {
    public:
        explicit StubParser(std::unique_ptr<DataNode> rootNode) : root(std::move(rootNode)) {}

        mutable std::string lastPath;

        std::unique_ptr<DataNode> parse(const std::string &filePath) const override
        {
            this->lastPath = filePath;

            auto *typed = dynamic_cast<StubNode *>(this->root.get());
            if (!typed) throw std::runtime_error("Unexpected root node type in fake parser");

            return std::make_unique<StubNode>(*typed);
        }

        static std::unique_ptr<DataNode> makeCombosRoot()
        {
            auto step1 = std::make_unique<StubNode>();
            step1->setString("action", "Attack");
            step1->setFloat("maxDelay", 150.0f);

            auto step2 = std::make_unique<StubNode>();
            step2->setString("action", "Jump");
            step2->setFloat("maxDelay", 100.0f);

            auto comboA = std::make_unique<StubNode>();
            comboA->setString("name", "uppercut");
            comboA->setInt("priority", 10);
            comboA->setBool("consume", false);

            std::vector<std::unique_ptr<DataNode>> comboASteps;
            comboASteps.push_back(std::move(step1));
            comboASteps.push_back(std::move(step2));
            comboA->setArray("steps", std::move(comboASteps));

            auto step3 = std::make_unique<StubNode>();
            step3->setString("action", "Defend");
            step3->setFloat("maxDelay", 80.0f);

            auto comboB = std::make_unique<StubNode>();
            comboB->setString("name", "guard");

            std::vector<std::unique_ptr<DataNode>> comboBSteps;
            comboBSteps.push_back(std::move(step3));
            comboB->setArray("steps", std::move(comboBSteps));

            auto rootNode = std::make_unique<StubNode>();
            std::vector<std::unique_ptr<DataNode>> combos;
            combos.push_back(std::move(comboA));
            combos.push_back(std::move(comboB));
            rootNode->setArray("combos", std::move(combos));

            return rootNode;
        }

    private:
        std::unique_ptr<DataNode> root;
    };
};

TEST_CASE_METHOD(Stub, "ComboLoader parses combos with optional fields",
    "[unit][combo_loader]"
) {
    StubParser parser(StubParser::makeCombosRoot());
    ComboLoader loader(parser);

    const auto combos = loader.load("assets/inputs/combos.json");

    REQUIRE(combos.size() == 2);

    REQUIRE(combos[0].name == "uppercut");
    REQUIRE(combos[0].priority == 10);
    REQUIRE(combos[0].consumeInput == false);
    REQUIRE(combos[0].steps.size() == 2);
    REQUIRE(combos[0].steps[0].action == InputAction::Attack);
    REQUIRE(combos[0].steps[0].maxDelay == 150);
    REQUIRE(combos[0].steps[1].action == InputAction::Jump);
    REQUIRE(combos[0].steps[1].maxDelay == 100);

    REQUIRE(combos[1].name == "guard");
    REQUIRE(combos[1].priority == 0);
    REQUIRE(combos[1].consumeInput == true);
    REQUIRE(combos[1].steps.size() == 1);
    REQUIRE(combos[1].steps[0].action == InputAction::Defend);
    REQUIRE(combos[1].steps[0].maxDelay == 80);
}

TEST_CASE_METHOD(Stub, "ComboLoader forwards file path to parser",
    "[unit][combo_loader]"
) {
    StubParser parser(StubParser::makeCombosRoot());
    ComboLoader loader(parser);

    (void)loader.load("custom/combos.json");

    REQUIRE(parser.lastPath == "custom/combos.json");
}
