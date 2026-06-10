#ifndef stub_platform_event_provider_h
#define stub_platform_event_provider_h

#include "../../src/engine/include/IPlatformEventProvider/IPlatformEventProvider.h"

#include <vector>
#include <memory>

class StubPlatformEventProvider : public IPlatformEventProvider
{
public:
    int pollCalls = 0;
    std::vector<std::unique_ptr<PlatformEvent>> platformEvents;

    std::vector<std::unique_ptr<PlatformEvent>> pollEvents() override
    {
        ++this->pollCalls;
        std::vector<std::unique_ptr<PlatformEvent>> result;
        for (auto& e : this->platformEvents) result.push_back(e->clone());
        
        return result;
    }
};

#endif // stub_platform_event_provider_h
