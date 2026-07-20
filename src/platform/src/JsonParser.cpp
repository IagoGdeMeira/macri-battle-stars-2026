#include "JsonParser/JsonParser.h"

#include "JsonNode/JsonNode.h"

#include "engine/include/PathResolver/PathResolver.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

std::unique_ptr<DataNode> JsonParser::parse(const std::string& path) const
{
    std::ifstream file(PathResolver::resolve(path));
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + path);

    json data = json::parse(file);
    return std::make_unique<JsonNode>(data);
}
