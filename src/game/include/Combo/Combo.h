#ifndef combo_h
#define combo_h

#include "../../domain/include/InputAction/InputAction.h"
#include "../../domain/include/TriggerId/TriggerId.h"

#include <string>
#include <vector>

struct ComboStep { InputAction action; float maxDelay; };

struct Combo
{
    std::string name;

    TriggerId trigger = TriggerId::Unknown;
    
    std::vector<ComboStep> steps;

    int priority = 0;
    bool consumeInput = true;
};

#endif // combo_h
