#ifndef stub_data_node_h
#define stub_data_node_h

#include "../../src/engine/include/DataNode/DataNode.h"

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

    bool has(const std::string& key) const override
    {
        if (this->ints.count(key)) return true;
        if (this->floats.count(key)) return true;
        if (this->bools.count(key)) return true;
        if (this->strings.count(key)) return true;
        if (this->objects.count(key)) return true;
        if (this->arrays.count(key)) return true;
        return false;
    }

    std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override
    {
        auto it = this->strings.find(key);
        return (it != this->strings.end()) ? it->second : fallback;
    }

    int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override
    {
        auto it = this->ints.find(key);
        return (it != this->ints.end()) ? it->second : fallback;
    }

    float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override
     {
        auto it = this->floats.find(key);
        return (it != this->floats.end()) ? it->second : fallback;
    }

    bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override
    {
        auto it = this->bools.find(key);
        return (it != this->bools.end()) ? it->second : fallback;
    }

    std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override
    {
        auto it = this->arrays.find(key);
        if (it == this->arrays.end()) throw std::runtime_error("Missing array key: " + key);
        
        std::vector<std::unique_ptr<DataNode>> out;
        out.reserve(it->second.size());

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
        if (key.empty()) throw std::runtime_error("StubDataNode: root value not supported");
        this->strings[key] = value;
    }

    void setInt(const std::string& key, int value) override
    {
        if (key.empty()) throw std::runtime_error("StubDataNode: root value not supported");
        this->ints[key] = value;
    }

    void setFloat(const std::string& key, float value) override
    {
        if (key.empty()) throw std::runtime_error("StubDataNode: root value not supported");
        this->floats[key] = value;
    }

    void setBool(const std::string& key, bool value) override
    {
        if (key.empty()) throw std::runtime_error("StubDataNode: root value not supported");
        this->bools[key] = value;
    }

    void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value) override
    {
        std::vector<StubDataNode> converted;
        converted.reserve(value.size());
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
        auto* typed = dynamic_cast<StubDataNode*>(value.get());
        if (!typed) throw std::runtime_error("StubDataNode::setObject: node is not StubDataNode");
        this->objects[key] = *typed;
    }

    std::unique_ptr<DataNode> clone() const override { return std::make_unique<StubDataNode>(*this); }

private:
    std::unordered_map<std::string, std::string> strings;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, std::vector<StubDataNode>> arrays;
    std::unordered_map<std::string, StubDataNode> objects;
};

#endif // stub_data_node_h
