#ifndef local_to_world_system_h
#define local_to_world_system_h

#include "../../engine/include/System/System.h"

class LocalToWorldSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // local_to_world_system_h
