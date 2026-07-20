#ifndef stub_data_node_h
#define stub_data_node_h

#include "engine/include/DataNode/DataNode.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class StubDataNode : public DataNode
{
public:
    StubDataNode() = default;
    StubDataNode(const StubDataNode& other) = default;

    void setRootString(const std::string& value) { this->rootString = value; this->hasRootString = true; }
    void setRootInt(int value) { this->rootInt = value; this->hasRootInt = true; }
    void setRootFloat(float value) { this->rootFloat = value; this->hasRootFloat = true; }
    void setRootBool(bool value) { this->rootBool = value; this->hasRootBool = true; }

    bool has(const std::string& key) const override
    {
        if (key.empty()) return this->hasRootString || this->hasRootInt || this->hasRootFloat || this->hasRootBool;
        
        if (this->strings.count(key)) return true;
        if (this->ints.count(key)) return true;
        if (this->floats.count(key)) return true;
        if (this->bools.count(key)) return true;
        if (this->arrays.count(key)) return true;
        if (this->objects.count(key)) return true;
        return false;
    }

    std::string getString(const std::string& key, const std::string& fallback = defaultStringFallback) const override
    {
        if (key.empty())
        {
            if (this->hasRootString) return this->rootString;
            if (this->hasRootInt) return std::to_string(this->rootInt);
            if (this->hasRootFloat) return std::to_string(this->rootFloat);
            if (this->hasRootBool) return this->rootBool ? "true" : "false";
            return fallback;
        }
        auto it = strings.find(key);
        return (it != strings.end()) ? it->second : fallback;
    }

    int getInt(const std::string& key, const int& fallback = defaultIntFallback) const override
    {
        if (key.empty())
        {
            if (this->hasRootInt) return this->rootInt;
            if (this->hasRootFloat) return static_cast<int>(this->rootFloat);
            if (this->hasRootBool) return this->rootBool ? 1 : 0;
            return fallback;
        }
        auto it = ints.find(key);
        return (it != ints.end()) ? it->second : fallback;
    }

    float getFloat(const std::string& key, const float& fallback = defaultFloatFallback) const override
    {
        if (key.empty())
        {
            if (this->hasRootFloat) return this->rootFloat;
            if (this->hasRootInt) return static_cast<float>(this->rootInt);
            return fallback;
        }
        auto it = floats.find(key);
        return (it != floats.end()) ? it->second : fallback;
    }

    bool getBool(const std::string& key, const bool& fallback = defaultBoolFallback) const override
    {
        if (key.empty())
        {
            if (this->hasRootBool) return this->rootBool;
            if (this->hasRootInt) return this->rootInt != 0;
            if (this->hasRootFloat) return this->rootFloat != 0.f;
            return fallback;
        }
        auto it = bools.find(key);
        return (it != bools.end()) ? it->second : fallback;
    }

    std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
    {
        auto it = this->arrays.find(key);
        if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);
        std::vector<std::unique_ptr<DataNode>> out;

        for (const auto& node : it->second) out.push_back(std::make_unique<StubDataNode>(node));
        return out;
    }

    std::unique_ptr<DataNode> getObject(const std::string& key) const override
    {
        auto it = this->objects.find(key);
        if (it == this->objects.end()) return nullptr;
        return std::make_unique<StubDataNode>(it->second);
    }

    void setString(const std::string& key, const std::string& value) override
    {
        if (key.empty()) { this->setRootString(value); return; }
        this->strings[key] = value;
    }

    void setInt(const std::string& key, int value) override
    {
        if (key.empty()) { this->setRootInt(value); return; }
        this->ints[key] = value;
    }

    void setFloat(const std::string& key, float value) override
    {
        if (key.empty()) { this->setRootFloat(value); return; }
        this->floats[key] = value;
    }

    void setBool(const std::string& key, bool value) override
    {
        if (key.empty()) { this->setRootBool(value); return; }
        this->bools[key] = value;
    }

    void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value) override
    {
        if (key.empty()) throw std::runtime_error("StubDataNode: root array not supported");
        
        std::vector<StubDataNode> converted;
        for (auto& node : value)
        {
            auto* typed = dynamic_cast<StubDataNode*>(node.get());
            if (!typed) throw std::runtime_error("StubDataNode::setArray: node is not StubDataNode");
            converted.push_back(*typed);
        }
        this->arrays[key] = std::move(converted);
    }

    void setObject(const std::string& key, std::unique_ptr<DataNode> value) override
    {
        if (key.empty()) throw std::runtime_error("StubDataNode: root object not supported");
        
        auto* typed = dynamic_cast<StubDataNode*>(value.get());
        if (!typed) throw std::runtime_error("StubDataNode::setObject: node is not StubDataNode");
        this->objects[key] = *typed;
    }

    std::unique_ptr<DataNode> clone() const override { return std::make_unique<StubDataNode>(*this); }

private:
    std::string rootString;
    int rootInt = 0;
    float rootFloat = 0.f;
    bool rootBool = false;
    bool hasRootString = false, hasRootInt = false, hasRootFloat = false, hasRootBool = false;

    std::unordered_map<std::string, std::string> strings;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, std::vector<StubDataNode>> arrays;
    std::unordered_map<std::string, StubDataNode> objects;
};

#endif // stub_data_node_h
