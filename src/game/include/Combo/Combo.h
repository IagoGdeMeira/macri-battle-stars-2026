#ifndef combo_h
#define combo_h

#include <string>
#include <vector>

#include "../../domain/include/InputAction/InputAction.h"

struct ComboStep
{
    InputAction action;
    float maxDelay;
};

struct Combo
{
    std::string name;

    std::vector<ComboStep> steps;

    int priority = 0;

    bool consumeInput = true;
};

#endif // combo_h
