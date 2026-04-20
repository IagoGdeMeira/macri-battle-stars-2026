#include "../include/MapRosterLoader/MapRosterLoader.h"

#include <stdexcept>

MapRoster MapRosterLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);
    MapRoster roster;

    for (auto& item : root->getArray("maps"))
    {
        MapEntry entry;
        entry.id = item->getString("id");
        entry.name = item->getString("name");
        entry.definitionPath = item->getString("definition");
        roster.addEntry(entry);
    }

    return roster;
}
