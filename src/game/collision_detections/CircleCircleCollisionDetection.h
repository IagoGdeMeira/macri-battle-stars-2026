#ifndef circle_circle_collision_detection_h
#define circle_circle_collision_detection_h

#include "ICollisionDetection/ICollisionDetection.h"

class CircleCircleCollisionDetection : public ICollisionDetection
{
public:
    void detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx) override;

private:
    bool circleToCircle(Entity a, Entity b, UpdateContext& ctx);
};

#endif // circle_circle_collision_detection_h
