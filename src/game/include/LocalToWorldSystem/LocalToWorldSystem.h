#ifndef local_to_world_system_h
#define local_to_world_system_h

#include "../../engine/include/System/System.h"

struct LocalTransform;
struct TransformComponent;

class LocalToWorldSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    static float rotateLocalX(const LocalTransform& local, float cosR, float sinR);
    static float rotateLocalY(const LocalTransform& local, float cosR, float sinR);
    static void applyParentTransform(
        TransformComponent& transform,
        const LocalTransform& local,
        const TransformComponent& parentTransform);
};

#endif // local_to_world_system_h
