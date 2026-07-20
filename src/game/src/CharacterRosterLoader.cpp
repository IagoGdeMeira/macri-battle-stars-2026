#include "CharacterRosterLoader/CharacterRosterLoader.h"

CharacterRoster CharacterRosterLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);
    CharacterRoster roster;

    for (auto& item : root->getArray("characters"))
    {
        CharacterRoster::Entry entry;
        entry.id = item->getString("id");
        entry.name = item->getString("name");
        entry.definitionPath = item->getString("definition");
        roster.addEntry(entry);
    }

    return roster;
}
