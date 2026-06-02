#ifndef map_roster_loader_h
#define map_roster_loader_h

#include "../MapRoster/MapRoster.h"

#include "../../engine/include/DataParser/DataParser.h"

class MapRosterLoader
{
public:
    explicit MapRosterLoader(DataParser& parser) : parser(parser) {}

    MapRoster load(const std::string& path) const;

private:
    DataParser& parser;
};

#endif // map_roster_loader_h
