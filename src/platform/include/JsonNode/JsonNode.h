#ifndef json_node_h
#define json_node_h

#include "engine/include/DataNode/DataNode.h"

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class JsonNode : public DataNode
{
public:
    JsonNode(const json& data) : data(data) {}

    bool has(const std::string& key) const override;

    std::string getString(const std::string& key, const std::string& fallback = DataNode::defaultStringFallback) const override;
    int getInt(const std::string& key, const int& fallback = DataNode::defaultIntFallback) const override;
    float getFloat(const std::string& key, const float& fallback = DataNode::defaultFloatFallback) const override;
    bool getBool(const std::string& key, const bool& fallback = DataNode::defaultBoolFallback) const override;
    std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override;
    std::unique_ptr<DataNode> getObject(const std::string& key) const override;

    void setString(const std::string& key, const std::string& value) override;
    void setInt(const std::string& key, int value) override;
    void setFloat(const std::string& key, float value) override;
    void setBool(const std::string& key, bool value) override;
    void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value) override;
    void setObject(const std::string& key, std::unique_ptr<DataNode> value) override;

    virtual bool isString() const override;
    virtual bool isInt() const override;
    virtual bool isFloat() const override;
    virtual bool isBool() const override;
    virtual bool isArray() const override;
    virtual bool isObject() const override;

    std::unique_ptr<DataNode> clone() const override;

private:
    json data;
};

#endif // json_node_h
