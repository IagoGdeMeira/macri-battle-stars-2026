#ifndef combo_h
#define combo_h

#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include <string>
#include <vector>

struct Combo
{
    struct Step { InputAction action; float maxDelay; };

    std::string name;
    TriggerId trigger = TriggerId::Unknown;
    std::vector<Step> steps;
    int priority = 0;
    bool consumeInput = true;
};

#endif // combo_h
