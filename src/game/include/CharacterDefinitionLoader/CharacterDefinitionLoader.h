#ifndef character_definition_loader_h
#define character_definition_loader_h

#include "../CharacterDefinition/CharacterDefinition.h"

#include "../../engine/include/DataParser/DataParser.h"

class CharacterDefinitionLoader
{
public:
    CharacterDefinitionLoader(DataParser& parser) : parser(parser) {}

    CharacterDefinition load(const std::string& path) const;

private:
    DataParser& parser;
};

#endif // character_definition_loader_h
