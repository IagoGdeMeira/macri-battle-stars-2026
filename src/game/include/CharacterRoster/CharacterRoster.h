#ifndef character_roster_h
#define character_roster_h

#include <string>
#include <unordered_map>
#include <vector>

struct CharacterEntry { std::string id, name, definitionPath; };

class CharacterRoster
{
public:
    void addEntry(const CharacterEntry& entry);
    
    const CharacterEntry* findById(const std::string& id) const;

    const std::vector<CharacterEntry>& getAll() const { return this->entries; }

private:
    std::vector<CharacterEntry> entries;
    std::unordered_map<std::string, size_t> idMap;
};

#endif
