#include "../../src/game/include/ConditionFactory/ConditionFactory.h"

#include "../../src/engine/include/DataNode/DataNode.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class ConditionFactoryFixture
{
public:
    class Node : public DataNode
    {
    public:
        void setString(const std::string& key, const std::string& value)
        { this->strings[key] = value; }

        void setFloat(const std::string& key, float value)
        { this->floats[key] = value; }

        bool has(const std::string& key) const override
        { return this->strings.contains(key) || this->floats.contains(key); }

        std::string getString(const std::string& key) const override
        {
            const auto it = this->strings.find(key);
            if (it == this->strings.end()) throw std::runtime_error("Missing string key: " + key);
            return it->second;
        }

        int getInt(const std::string& key) const override
        { throw std::runtime_error("Unexpected int key: " + key); }

        float getFloat(const std::string& key) const override
        {
            const auto it = this->floats.find(key);
            if (it == this->floats.end()) throw std::runtime_error("Missing float key: " + key);
            return it->second;
        }

        bool getBool(const std::string& key) const override
        { throw std::runtime_error("Unexpected bool key: " + key); }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        { throw std::runtime_error("Unexpected array key: " + key); }

    private:
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, float> floats;
    };

    Node node;
};

TEST_CASE_METHOD(ConditionFactoryFixture, "ConditionFactory creates min_time conditions",
    "[unit][condition_factory]"
) {
    this->node.setString("type", "min_time");
    this->node.setFloat("value", 0.5f);

    auto condition = ConditionFactory::create(this->node);

    REQUIRE(condition != nullptr);
}

TEST_CASE_METHOD(ConditionFactoryFixture, "ConditionFactory creates input_pressed conditions",
    "[unit][condition_factory]"
) {
    this->node.setString("type", "input_pressed");
    this->node.setString("action", "Attack");

    auto condition = ConditionFactory::create(this->node);

    REQUIRE(condition != nullptr);
}

TEST_CASE_METHOD(ConditionFactoryFixture, "ConditionFactory rejects unknown condition types",
    "[unit][condition_factory]"
) {
    this->node.setString("type", "unknown");

    REQUIRE_THROWS_AS(ConditionFactory::create(this->node), std::runtime_error);
}
