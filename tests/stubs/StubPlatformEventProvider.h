#ifndef stub_platform_event_provider_h
#define stub_platform_event_provider_h

#include "engine/include/IPlatformEventProvider/IPlatformEventProvider.h"

#include <vector>
#include <memory>

class StubPlatformEventProvider : public IPlatformEventProvider
{
public:
    int pollCalls = 0;
    std::vector<IPlatformEventProvider::Event> platformEvents;

    std::vector<IPlatformEventProvider::Event> pollEvents() override
    {
        ++this->pollCalls;
        std::vector<IPlatformEventProvider::Event> result;
        for (auto& e : this->platformEvents) result.push_back(e->clone());
        
        return result;
    }
};

#endif // stub_platform_event_provider_h
