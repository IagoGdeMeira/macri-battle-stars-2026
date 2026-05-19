#include "../include/JsonNode/JsonNode.h"

bool JsonNode::has(const std::string& key) const { return this->data.contains(key); }

std::string JsonNode::getString(const std::string& key, const std::string& fallback) const
{
    if (key.empty())
    {
        if (!this->data.is_null()) return this->data.get<std::string>();
        return fallback;
    }

    if (!this->data.contains(key))return fallback;

    return this->data.at(key).get<std::string>();
}

int JsonNode::getInt(const std::string& key, const int& fallback) const
{
    if (key.empty())
    {
        if (!this->data.is_null()) return this->data.get<int>();
        return fallback;
    }

    if (!this->data.contains(key)) return fallback;

    return this->data.at(key).get<int>();
}

float JsonNode::getFloat(const std::string& key, const float& fallback) const
{
    if (key.empty())
    {
        if (!this->data.is_null()) return this->data.get<float>();
        return fallback;
    }

    if (!this->data.contains(key)) return fallback;

    return this->data.at(key).get<float>();
}

bool JsonNode::getBool(const std::string& key, const bool& fallback) const
{
    if (key.empty())
    {
        if (!this->data.is_null()) return this->data.get<bool>();
        return fallback;
    }

    if (!this->data.contains(key)) return fallback;

    return this->data.at(key).get<bool>();
}

std::vector<std::unique_ptr<DataNode>> JsonNode::getArray(const std::string& key) const
{
    if (!this->data.contains(key)) throw std::runtime_error("Key not found: " + key);

    const auto& node = this->data.at(key);
    if (!node.is_array()) throw std::runtime_error("Key is not an array: " + key);

    std::vector<std::unique_ptr<DataNode>> result;

    for (const auto& item : node)
    { result.push_back(std::make_unique<JsonNode>(item)); }

    return result;
}

std::unique_ptr<DataNode> JsonNode::getObject(const std::string& key) const
{
    if (!this->data.contains(key)) return nullptr;

    const auto& node = this->data.at(key);
    if (!node.is_object()) return nullptr;

    return std::make_unique<JsonNode>(node);
}
