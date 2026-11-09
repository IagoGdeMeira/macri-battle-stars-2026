#ifndef sdl_platform_event_provider_h
#define sdl_platform_event_provider_h

#include "engine/include/IPlatformEventProvider/IPlatformEventProvider.h"

class SDLPlatformEventProvider : public IPlatformEventProvider
{
public:
    std::vector<IPlatformEventProvider::Event> pollEvents() override;
};

#endif // sdl_platform_event_provider_h
