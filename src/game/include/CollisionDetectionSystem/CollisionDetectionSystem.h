#ifndef collision_detection_system_h
#define collision_detection_system_h

#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/System/System.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

struct CircleColliderComponent;
struct RectangleColliderComponent;
struct TransformComponent;

class CollisionDetectionSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    struct CollisionPair { Entity a, b; };
    struct Cell { std::vector<Entity> entities; };

    struct RectParams { const TransformComponent& transform; const RectangleColliderComponent& collider; };
    struct CircleParams { const TransformComponent& transform; const CircleColliderComponent& collider; };
    struct DetectionParams { UpdateContext& ctx; std::vector<CollisionPair>& pairs; };

    using Grid = std::unordered_map<long long, Cell>;
    using PairSet = std::unordered_set<unsigned long long>;

    static long long hash(int x, int y);
    static unsigned long long hashPair(Entity a, Entity b);

    void buildGrid(UpdateContext& ctx, Grid& grid, float cellSize);
    void detect(UpdateContext& ctx, std::vector<CollisionPair>& pairs);

    void detectInCell(DetectionParams params, const Cell& cell, PairSet& checkedPairs);
    void detectInPair(DetectionParams params, CollisionPair pair, PairSet& checkedPairs);

    static bool rectToRect(RectParams a, RectParams b);
    static bool circleToCircle(CircleParams a, CircleParams b);
    static bool rectToCircle(RectParams rect, CircleParams circle);
};

#endif // collision_detection_system_h
