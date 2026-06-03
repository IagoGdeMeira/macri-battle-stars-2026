#include "../../src/engine/include/DataUtils/DataUtils.h"

#include "../../src/engine/include/DataNode/DataNode.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class DataUtilsFixture
{
public:
    class StubNode : public DataNode
    {
    public:
        StubNode() = default;

        bool has(const std::string& key) const override
        {
            if (this->floats.count(key))    return true;
            if (this->ints.count(key))      return true;
            if (this->bools.count(key))     return true;
            if (this->strings.count(key))   return true;
            if (this->objects.count(key))   return true;
            if (this->arrays.count(key))    return true;
            return false;
        }

        std::string getString(const std::string& key,
            const std::string& fallback = DataNode::defaultStringFallback
        ) const override
        {
            auto it = this->strings.find(key);
            return it != this->strings.end() ? it->second : fallback;
        }

        int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
        {
            auto it = this->ints.find(key);
            if (it != this->ints.end()) return it->second;
            auto fit = this->floats.find(key);
            if (fit != this->floats.end()) return static_cast<int>(fit->second);
            return fallback;
        }

        float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
        {
            auto it = this->floats.find(key);
            if (it != this->floats.end()) return it->second;
            auto iit = this->ints.find(key);
            if (iit != this->ints.end()) return static_cast<float>(iit->second);
            return fallback;
        }

        bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
        {
            auto it = this->bools.find(key);
            return it != this->bools.end() ? it->second : fallback;
        }

        std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
        {
            const auto it = this->arrays.find(key);
            if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);

            std::vector<std::unique_ptr<DataNode>> out;
            out.reserve(it->second.size());

            for (const auto& node : it->second)
            {
                auto* typed = dynamic_cast<StubNode*>(node.get());
                if (!typed) throw std::runtime_error("Unexpected node type in StubNode array");
                out.push_back(clone(*typed));
            }

            return out;
        }

        std::unique_ptr<DataNode> getObject(const std::string& key) const override
        {
            auto it = this->objects.find(key);
            if (it == this->objects.end()) return nullptr;
            return clone(*it->second);
        }

        void setFloat(const std::string& key, float v) { this->floats[key] = v; }
        void setInt(const std::string& key, int v) { this->ints[key] = v; }
        void setBool(const std::string& key, bool v) { this->bools[key] = v; }
        void setString(const std::string& key, const std::string& v) { this->strings[key] = v; }
        void setObject(const std::string& key, std::unique_ptr<StubNode> obj) { this->objects[key] = std::move(obj); }
        void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> arr) { this->arrays[key] = std::move(arr); }

    private:
        static std::unique_ptr<StubNode> clone(const StubNode& source)
        {
            auto copy = std::make_unique<StubNode>();
            copy->floats = source.floats;
            copy->ints = source.ints;
            copy->bools = source.bools;
            copy->strings = source.strings;

            for (const auto& [key, value] : source.objects) copy->objects[key] = clone(*value);
            return copy;
        }

        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, std::string> strings;
        std::unordered_map<std::string, std::unique_ptr<StubNode>> objects;
        std::unordered_map<std::string, std::vector<std::unique_ptr<DataNode>>> arrays;
    };
};

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parsePosition reads x and y",
    "[unit][data_utils]")
{
    StubNode node;
    node.setFloat("x", 1.5f);
    node.setFloat("y", 2.25f);

    Position result = DataUtils::parsePosition(node, {0.f, 0.f});

    REQUIRE(result.x == 1.5f);
    REQUIRE(result.y == 2.25f);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parsePosition returns default when missing",
    "[unit][data_utils]")
{
    StubNode node;
    node.setFloat("x", 1.5f);

    Position def{5.f, 6.f};

    Position result = DataUtils::parsePosition(node, def);

    REQUIRE(result.x == def.x);
    REQUIRE(result.y == def.y);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parseSize reads w and h",
    "[unit][data_utils]")
{
    StubNode node;
    node.setFloat("w", 10.f);
    node.setFloat("h", 20.f);

    Dimension2D result = DataUtils::parseSize(node, {0.f, 0.f});

    REQUIRE(result.width == 10.f);
    REQUIRE(result.height == 20.f);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parseRect reads nested position and size",
    "[unit][data_utils]")
{
    StubNode node;

    auto pos = std::make_unique<StubNode>();
    pos->setFloat("x", 3.f);
    pos->setFloat("y", 4.f);
    node.setObject("position", std::move(pos));

    auto size = std::make_unique<StubNode>();
    size->setFloat("w", 7.f);
    size->setFloat("h", 8.f);
    node.setObject("size", std::move(size));

    Rectangle def{{0.f,0.f},{0.f,0.f}};

    Rectangle result = DataUtils::parseRect(node, def);

    REQUIRE(result.position.x == 3.f);
    REQUIRE(result.position.y == 4.f);
    REQUIRE(result.size.width == 7.f);
    REQUIRE(result.size.height == 8.f);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parseColor reads r,g,b,a",
    "[unit][data_utils]")
{
    StubNode node;
    node.setInt("r", 10);
    node.setInt("g", 20);
    node.setInt("b", 30);
    node.setInt("a", 128);

    Color def{255,255,255,255};

    Color result = DataUtils::parseColor(node, def);

    REQUIRE(result.r == 10);
    REQUIRE(result.g == 20);
    REQUIRE(result.b == 30);
    REQUIRE(result.a == 128);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parseAABB reads left,right,top,bottom",
    "[unit][data_utils]")
{
    StubNode node;
    node.setFloat("left", 1.f);
    node.setFloat("right", 2.f);
    node.setFloat("top", 3.f);
    node.setFloat("bottom", 4.f);

    AABB result = DataUtils::parseAABB(node, {0.f,0.f,0.f,0.f});

    REQUIRE(result.left == 1.f);
    REQUIRE(result.right == 2.f);
    REQUIRE(result.top == 3.f);
    REQUIRE(result.bottom == 4.f);
}

TEST_CASE_METHOD(DataUtilsFixture, "DataUtils::parseCorners reads topLeft, topRight, bottomLeft, bottomRight",
    "[unit][data_utils]"
) {
    StubNode node;
    node.setFloat("topLeft", 1.f);
    node.setFloat("topRight", 3.f);
    node.setFloat("bottomLeft", 5.f);
    node.setFloat("bottomRight", 7.f);

    Corners result = DataUtils::parseCorners(node, {0.f,0.f,0.f,0.f});

    REQUIRE(result.topLeft == 1.f);
    REQUIRE(result.topRight == 3.f);
    REQUIRE(result.bottomLeft == 5.f);
    REQUIRE(result.bottomRight == 7.f);
}
