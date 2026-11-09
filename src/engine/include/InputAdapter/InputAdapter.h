#ifndef input_adapter_h
#define input_adapter_h

#include "PlatformEvent.h"

#include <memory>
#include <vector>

class InputAdapter
{
public:
    using Event = std::unique_ptr<PlatformEvent>;

    virtual ~InputAdapter() = default;
    virtual void processEvents(const std::vector<Event>& events) = 0;
};

#endif // input_adapter_h
