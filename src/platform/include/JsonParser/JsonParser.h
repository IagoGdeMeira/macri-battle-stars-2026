#ifndef json_parser_h
#define json_parser_h

#include "../../engine/include/DataParser/DataParser.h"

class JsonParser : public DataParser
{
public:
    std::unique_ptr<DataNode> parse(const std::string& path) const override;
};

#endif // json_parser_h
