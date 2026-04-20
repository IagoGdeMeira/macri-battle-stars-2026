#ifndef map_loader_h
#define map_loader_h

#include "../MapData/MapData.h"

#include "../../../engine/include/DataParser/DataParser.h"

class MapLoader
{
public:
    explicit MapLoader(DataParser& parser) : parser(parser) {}

    MapData load(const std::string& path) const;

private:
    DataParser& parser;
};

#endif // map_loader_h
