#ifndef rect_rect_collision_detection_h
#define rect_rect_collision_detection_h

#include "ICollisionDetection/ICollisionDetection.h"

#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"

#include <vector>

class RectRectCollisionDetection : public ICollisionDetection
{
public:
    void detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx) override;

private:
    bool rectToRect(Entity a, Entity b, UpdateContext& ctx);
};

#endif // rect_rect_collision_detection_h
