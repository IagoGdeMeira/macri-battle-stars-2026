#ifndef sdl_platform_event_provider_h
#define sdl_platform_event_provider_h

#include "engine/include/IPlatformEventProvider/IPlatformEventProvider.h"

class SDLPlatformEventProvider : public IPlatformEventProvider
{
public:
    std::vector<std::unique_ptr<PlatformEvent>> pollEvents() override;
};

#endif // sdl_platform_event_provider_h
