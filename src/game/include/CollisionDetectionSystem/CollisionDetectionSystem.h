#ifndef collision_detection_system_h
#define collision_detection_system_h

#include "ICollisionDetection/ICollisionDetection.h"

#include "engine/include/System/System.h"

#include <vector>
#include <memory>

class CollisionDetectionSystem : public System
{
public:
    explicit CollisionDetectionSystem(int updateInterval = 2) : updateInterval(updateInterval) {}

    void update(UpdateContext& ctx) override;
    void addDetector(std::unique_ptr<ICollisionDetection> detector) { this->detectors.push_back(std::move(detector)); }

private:
    struct EntityAABB { Entity entity; float minX = 0.f, maxX = 0.f, minY = 0.f, maxY = 0.f; };

    int updateInterval = 2;
    int frameCounter = 0;
    std::vector<EntityAABB> aabbs;
    std::vector<std::unique_ptr<ICollisionDetection>> detectors;

    void updateAABBs(UpdateContext& ctx);
    void sweepAndPrune(std::vector<ICollisionDetection::CollisionPair>& outPairs);
    void insertionSortAABBs();
};

#endif // collision_detection_system_h
