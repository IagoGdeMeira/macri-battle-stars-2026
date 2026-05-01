#ifndef input_manager_h
#define input_manager_h

#include "../InputAdapter/InputAdapter.h"
#include "../IPlatformEventProvider/IPlatformEventProvider.h"

#include <memory>
#include <vector>

class InputManager
{
public:
    InputManager() = default;

    void setProvider(std::unique_ptr<IPlatformEventProvider> provider);
    void addAdapter(std::unique_ptr<InputAdapter> adapter);
    void poll();

private:
    std::unique_ptr<IPlatformEventProvider> provider;
    std::vector<std::unique_ptr<InputAdapter>> adapters;
};

#endif // input_manager_h
