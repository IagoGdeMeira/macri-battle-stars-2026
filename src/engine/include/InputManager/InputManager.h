#ifndef input_manager_h
#define input_manager_h

#include "InputAdapter/InputAdapter.h"
#include "IPlatformEventProvider/IPlatformEventProvider.h"

#include <memory>
#include <vector>

class InputManager
{
public:
    InputManager() = default;

    using Provider = std::unique_ptr<IPlatformEventProvider>;
    using Adapter = std::unique_ptr<InputAdapter>;

    void setProvider(Provider provider);
    void addAdapter(Adapter adapter);
    void poll();

private:
    Provider provider;
    std::vector<Adapter> adapters;
};

#endif // input_manager_h
