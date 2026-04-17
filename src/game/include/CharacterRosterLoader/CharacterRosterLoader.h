#ifndef character_roster_loader_h
#define character_roster_loader_h

#include "../CharacterRoster/CharacterRoster.h"

#include "../../../engine/include/DataParser/DataParser.h"

class CharacterRosterLoader
{
public:
    explicit CharacterRosterLoader(DataParser& parser) : parser(parser) {}

    CharacterRoster load(const std::string& path) const;

private:
    DataParser& parser;
};

#endif
