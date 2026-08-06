#ifndef character_definition_h
#define character_definition_h

#include "JumpDefinition/JumpDefinition.h"

#include "domain/value_objects/Geometry/Geometry.h"

#include <string>
#include <vector>

struct CharacterDefinition
{
    std::string id;

    Dimension2D spriteSize {0.f, 0.f};
    std::string texturePath, animationsPath, stateMachinePath, combosPath, collisionsPath;

    JumpDefinition jump;
    
    std::vector<std::string> customStates;
};

#endif // character_definition_h
