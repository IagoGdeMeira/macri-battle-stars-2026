#ifndef data_parser_h
#define data_parser_h

#include "DataNode/DataNode.h"

#include <memory>
#include <string>

class DataParser
{
public:
    virtual ~DataParser() = default;

    virtual std::unique_ptr<DataNode> parse(const std::string& filePath) const = 0;
};

#endif // data_parser_h
