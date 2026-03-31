#ifndef json_node_h
#define json_node_h

#include "../../engine/include/DataNode/DataNode.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonNode : public DataNode
{
public:
    JsonNode(const json& data) : data(data) {}

    bool has(const std::string& key) const override;

    std::string getString(const std::string& key) const override;
    int getInt(const std::string& key) const override;
    float getFloat(const std::string& key) const override;
    std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const override;

private:
    json data;
};

#endif // json_node_h