#include "../include/CharacterRoster/CharacterRoster.h"

void CharacterRoster::addEntry(const CharacterEntry& entry)
{
    this->idMap[entry.id] = this->entries.size();
    this->entries.push_back(entry);
}

const CharacterEntry* CharacterRoster::findById(const std::string& id) const
{
    auto it = this->idMap.find(id);
    return (it != this->idMap.end()) ? &this->entries[it->second] : nullptr;
}
