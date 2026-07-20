#ifndef stub_data_parser_h
#define stub_data_parser_h

#include "StubDataNode.h"

#include "engine/include/DataParser/DataParser.h"

#include <unordered_map>

class StubDataParser : public DataParser
{
public:
    void registerNode(const std::string& path, std::unique_ptr<StubDataNode> node)
    { this->nodes[path] = std::move(node); }

    std::unique_ptr<DataNode> parse(const std::string& path) const override
    {
        auto it = this->nodes.find(path);
        if (it == this->nodes.end()) throw std::runtime_error("StubDataParser: path not registered: " + path);
        return it->second->clone();
    }

private:
    mutable std::unordered_map<std::string, std::unique_ptr<StubDataNode>> nodes;
};

#endif // stub_data_parser_h
