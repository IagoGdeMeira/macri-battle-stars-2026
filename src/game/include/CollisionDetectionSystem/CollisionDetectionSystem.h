#ifndef collision_detection_system_h
#define collision_detection_system_h

#include "../ICollisionDetection/ICollisionDetection.h"

#include "../../engine/include/System/System.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CollisionDetectionSystem : public System
{
public:
    explicit CollisionDetectionSystem(float cellSize = 500.f) : cellSize(cellSize) {}

    void addDetector(std::unique_ptr<ICollisionDetection> detector) { this->detectors.push_back(std::move(detector)); }
    void update(UpdateContext& ctx) override;

private:
    struct Cell { std::vector<Entity> entities; };

    using Grid = std::unordered_map<long long, Cell>;
    using PairSet = std::unordered_set<unsigned long long>;

    float cellSize;
    std::vector<std::unique_ptr<ICollisionDetection>> detectors;

    static long long hash(int x, int y) { return (static_cast<long long>(x) << 32) | static_cast<unsigned int>(y); }
    static unsigned long long hashPair(Entity a, Entity b);

    void buildGrid(UpdateContext& ctx, Grid& grid);
    void collectPairs(Grid& grid, std::vector<ICollisionDetection::CollisionPair>& outPairs);
};

#endif // collision_detection_system_h
