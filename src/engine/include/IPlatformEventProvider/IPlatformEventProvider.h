#ifndef i_platform_event_provider_h
#define i_platform_event_provider_h

#include "PlatformEvent.h"

#include <memory>
#include <vector>

class IPlatformEventProvider
{
public:
    using Event = std::unique_ptr<PlatformEvent>;

    virtual ~IPlatformEventProvider() = default;
    virtual std::vector<Event> pollEvents() = 0;
};

#endif // i_platform_event_provider_h
