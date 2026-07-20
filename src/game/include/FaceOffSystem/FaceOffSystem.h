#ifndef face_off_system_h
#define face_off_system_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

class FaceOffSystem : public System
{
public:
    struct PlayerInfo { Entity entity; float x; };

    explicit FaceOffSystem(EventBus& bus) : bus(bus) {}
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
};

#endif // face_off_system_h
