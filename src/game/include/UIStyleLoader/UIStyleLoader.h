#ifndef ui_style_loader_h
#define ui_style_loader_h

#include "engine/include/DataParser/DataParser.h"

#include <memory>
#include <string>
#include <unordered_map>

class DataNode;

class UIStyleLoader
{
public:
    using StyleMap = std::unordered_map<std::string, std::unique_ptr<DataNode>>;

    explicit UIStyleLoader(DataParser& parser) : parser(parser) {}
    StyleMap load(const std::string& path);

private:
    DataParser& parser;
};

#endif // ui_style_loader_h
