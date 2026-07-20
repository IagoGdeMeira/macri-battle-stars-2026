#ifndef map_roster_h
#define map_roster_h

#include <string>
#include <unordered_map>
#include <vector>

class MapRoster
{
public:
    struct Entry { std::string id, name, definitionPath; };

    void addEntry(const Entry& entry);

    const Entry* findById(const std::string& id) const;
    const std::vector<Entry>& getAll() const { return this->entries; }

private:
    std::vector<Entry> entries;
    std::unordered_map<std::string, size_t> idMap;
};

#endif // map_roster_h
