#ifndef character_definition_h
#define character_definition_h

#include <string>

struct CharacterDefinition
{
    std::string id;

    std::string texturePath;
    int spriteWidth = 0, spriteHeight = 0;

    std::string animationsPath;
    std::string stateMachinePath;
    std::string combosPath;
};

#endif // character_definition_h
