#ifndef input_adapter_h
#define input_adapter_h

#include "../events/PlatformEvent.h"

#include <memory>
#include <vector>

class InputAdapter
{
public:
    virtual ~InputAdapter() = default;

    virtual void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) = 0;
};

#endif // input_adapter_h
