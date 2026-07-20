#ifndef rect_circle_collision_detection_h
#define rect_circle_collision_detection_h

#include "ICollisionDetection/ICollisionDetection.h"

class RectCircleCollisionDetection : public ICollisionDetection
{
public:
    void detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx) override;

private:
    bool rectToCircle(Entity rect, Entity circle, UpdateContext& ctx);
};

#endif // rect_circle_collision_detection_h
