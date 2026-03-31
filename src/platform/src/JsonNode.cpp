#include "../include/JsonNode/JsonNode.h"

bool JsonNode::has(const std::string& key) const
{ return this->data.contains(key); }

std::string JsonNode::getString(const std::string& key) const
{
    if (!this->data.contains(key)) throw std::runtime_error("Key not found: " + key);
    return this->data.at(key).get<std::string>();
}

int JsonNode::getInt(const std::string& key) const
{
    if (!this->data.contains(key)) throw std::runtime_error("Key not found: " + key);
    return this->data.at(key).get<int>();
}

float JsonNode::getFloat(const std::string& key) const
{
    if (!this->data.contains(key)) throw std::runtime_error("Key not found: " + key);
    return this->data.at(key).get<float>();
}

std::vector<std::unique_ptr<DataNode>> JsonNode::getArray(const std::string& key) const
{
    if (!this->data.contains(key)) throw std::runtime_error("Key not found: " + key);

    std::vector<std::unique_ptr<DataNode>> result;

    for (const auto& item : this->data.at(key))
    { result.push_back(std::make_unique<JsonNode>(item)); }

    return result;
}
