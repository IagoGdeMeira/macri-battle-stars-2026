#ifndef i_platform_event_provider_h
#define i_platform_event_provider_h

#include "../events/PlatformEvent.h"

#include <memory>
#include <vector>

class IPlatformEventProvider
{
public:
    virtual ~IPlatformEventProvider() = default;
    
    virtual std::vector<std::unique_ptr<PlatformEvent>> pollEvents() = 0;
};

#endif // i_platform_event_provider_h
