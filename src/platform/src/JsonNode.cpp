#include "JsonNode/JsonNode.h"

#include <stdexcept>

bool JsonNode::has(const std::string& key) const { return this->data.contains(key); }

std::string JsonNode::getString(const std::string& key, const std::string& fallback) const
{
    if (key.empty())
    {
        if (this->data.is_string()) return this->data.get<std::string>();
        if (this->data.is_null()) return fallback;
        throw std::runtime_error("JsonNode::getString: root value is not a string");
    }

    auto it = this->data.find(key);
    if (it == this->data.end()) return fallback;
    if (it->is_string()) return it->get<std::string>();
    throw std::runtime_error("JsonNode::getString: value for key '" + key + "' is not a string");
}

int JsonNode::getInt(const std::string& key, const int& fallback) const
{
    if (key.empty())
    {
        if (this->data.is_number_integer()) return this->data.get<int>();
        if (this->data.is_null()) return fallback;
        throw std::runtime_error("JsonNode::getInt: root value is not an integer");
    }

    auto it = this->data.find(key);
    if (it == this->data.end()) return fallback;
    if (it->is_number_integer()) return it->get<int>();
    throw std::runtime_error("JsonNode::getInt: value for key '" + key + "' is not an integer");
}

float JsonNode::getFloat(const std::string& key, const float& fallback) const
{
    if (key.empty())
    {
        if (this->data.is_number_float() || this->data.is_number_integer()) return this->data.get<float>();
        if (this->data.is_null()) return fallback;
        throw std::runtime_error("JsonNode::getFloat: root value is not a float");
    }

    auto it = this->data.find(key);
    if (it == this->data.end()) return fallback;
    if (it->is_number_float() || it->is_number_integer()) return it->get<float>();
    throw std::runtime_error("JsonNode::getFloat: value for key '" + key + "' is not a float");
}

bool JsonNode::getBool(const std::string& key, const bool& fallback) const
{
    if (key.empty())
    {
        if (this->data.is_boolean()) return this->data.get<bool>();
        if (this->data.is_null()) return fallback;
        throw std::runtime_error("JsonNode::getBool: root value is not a boolean");
    }
    auto it = this->data.find(key);

    if (it == this->data.end()) return fallback;
    if (it->is_boolean()) return it->get<bool>();
    throw std::runtime_error("JsonNode::getBool: value for key '" + key + "' is not a boolean");
}

std::vector<std::unique_ptr<DataNode>> JsonNode::getArray(const std::string& key) const
{
    auto it = this->data.find(key);
    if (it == this->data.end()) throw std::runtime_error("Key not found: " + key);
    if (!it->is_array()) throw std::runtime_error("Key is not an array: " + key);
    
    std::vector<std::unique_ptr<DataNode>> result;
    for (const auto& item : *it) result.push_back(std::make_unique<JsonNode>(item));
    return result;
}

std::unique_ptr<DataNode> JsonNode::getObject(const std::string& key) const
{
    auto it = this->data.find(key);
    if (it == this->data.end()) return nullptr;
    if (!it->is_object()) return nullptr;
    return std::make_unique<JsonNode>(*it);
}

void JsonNode::setString(const std::string& key, const std::string& value)
{
    if (key.empty()) this->data = value;
    else this->data[key] = value;
}

void JsonNode::setInt(const std::string& key, int value)
{
    if (key.empty()) this->data = value;
    else this->data[key] = value;
}

void JsonNode::setFloat(const std::string& key, float value)
{
    if (key.empty()) this->data = value;
    else this->data[key] = value;
}

void JsonNode::setBool(const std::string& key, bool value)
{
    if (key.empty()) this->data = value;
    else this->data[key] = value;
}

void JsonNode::setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value)
{
    json arr = json::array();
    for (auto& node : value)
    {
        JsonNode* jsonNode = dynamic_cast<JsonNode*>(node.get());
        if (!jsonNode) throw std::runtime_error("Cannot convert non-JsonNode to JSON array");
        arr.push_back(jsonNode->data);
    }
    if (key.empty()) this->data = arr;
    else this->data[key] = arr;
}

void JsonNode::setObject(const std::string& key, std::unique_ptr<DataNode> value)
{
    JsonNode* jsonNode = dynamic_cast<JsonNode*>(value.get());
    if (!jsonNode) throw std::runtime_error("Cannot convert non-JsonNode to JSON object");
    if (key.empty()) this->data = jsonNode->data;
    else this->data[key] = jsonNode->data;
}

bool JsonNode::isString() const { return this->data.is_string(); }

bool JsonNode::isInt() const { return this->data.is_number_integer(); }

bool JsonNode::isFloat() const { return this->data.is_number_float() || this->isInt(); }

bool JsonNode::isBool() const { return this->data.is_boolean(); }

bool JsonNode::isArray() const { return this->data.is_array(); }

bool JsonNode::isObject() const { return this->data.is_object(); }

std::unique_ptr<DataNode> JsonNode::clone() const { return std::make_unique<JsonNode>(this->data); }
