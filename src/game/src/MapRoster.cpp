#include "../include/MapRoster/MapRoster.h"

void MapRoster::addEntry(const MapEntry& entry)
{
    this->idMap[entry.id] = this->entries.size();
    this->entries.push_back(entry);
}

const MapEntry* MapRoster::findById(const std::string& id) const
{
    auto it = this->idMap.find(id);
    return (it != this->idMap.end()) ? &this->entries[it->second] : nullptr;
}
