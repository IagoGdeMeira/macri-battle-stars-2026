#ifndef map_roster_h
#define map_roster_h

#include <string>
#include <unordered_map>
#include <vector>

struct MapEntry { std::string id, name, definitionPath; };

class MapRoster
{
public:
    void addEntry(const MapEntry& entry);

    const MapEntry* findById(const std::string& id) const;
    const std::vector<MapEntry>& getAll() const { return this->entries; }

private:
    std::vector<MapEntry> entries;
    std::unordered_map<std::string, size_t> idMap;
};

#endif // map_roster_h
