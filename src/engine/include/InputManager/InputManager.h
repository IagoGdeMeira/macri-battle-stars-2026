#ifndef input_manager_h
#define input_manager_h

#include "../InputAdapter/InputAdapter.h"

#include <memory>
#include <vector>

class InputManager
{
public:
    void addAdapter(std::unique_ptr<InputAdapter> adapter);
    void poll();

private:
    std::vector<std::unique_ptr<InputAdapter>> adapters;
};

#endif // input_manager_h
