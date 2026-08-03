#ifndef local_to_world_system_h
#define local_to_world_system_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"

struct TransformComponent;

class LocalToWorldSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    static void updateTransformRecursive(UpdateContext& ctx, Entity parent, const TransformComponent& parentTrans);
};

#endif // local_to_world_system_h
