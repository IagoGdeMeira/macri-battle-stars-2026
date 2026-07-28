#include "CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "CollisionEvent.h"

#include "domain/components/CircleColliderComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>

void CollisionDetectionSystem::update(UpdateContext& ctx)
{
    ++this->frameCounter;
    if (this->frameCounter % this->updateInterval != 0) return;

    this->updateAABBs(ctx);

    std::vector<ICollisionDetection::CollisionPair> pairs;
    this->sweepAndPrune(pairs);

    for (auto& detector : this->detectors) detector->detect(pairs, ctx);
}

void CollisionDetectionSystem::updateAABBs(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    this->aabbs.clear();
    this->aabbs.reserve(64);

    for (auto [e, t, r] : View<TransformComponent, RectangleColliderComponent>(comp))
    {
        AABB aabb;
        aabb.left = t.position.x - r.size.width * 0.5f;
        aabb.right = t.position.x + r.size.width * 0.5f;
        aabb.top = t.position.y - r.size.height * 0.5f;
        aabb.bottom = t.position.y + r.size.height * 0.5f;
        this->aabbs.push_back({e, aabb.left, aabb.right, aabb.top, aabb.bottom});
    }

    for (auto [e, t, c] : View<TransformComponent, CircleColliderComponent>(comp))
    {
        AABB aabb;
        aabb.left = t.position.x - c.radius;
        aabb.right = t.position.x + c.radius;
        aabb.top = t.position.y - c.radius;
        aabb.bottom = t.position.y + c.radius;
        this->aabbs.push_back({e, aabb.left, aabb.right, aabb.top, aabb.bottom});
    }

    this->insertionSortAABBs();
    LOG_DEBUG("CollisionDetectionSystem: collected {} AABBs", this->aabbs.size());
}

void CollisionDetectionSystem::insertionSortAABBs()
{
    if (this->aabbs.size() <= 1) return;

    for (size_t i = 1; i < this->aabbs.size(); ++i)
    {
        EntityAABB key = std::move(this->aabbs[i]);
        size_t j = i;
        while (j > 0 && this->aabbs[j - 1].minX > key.minX)
        {
            this->aabbs[j] = std::move(this->aabbs[j - 1]);
            --j;
        }
        this->aabbs[j] = std::move(key);
    }
}

void CollisionDetectionSystem::sweepAndPrune(std::vector<ICollisionDetection::CollisionPair>& outPairs)
{
    const size_t n = this->aabbs.size();
    outPairs.reserve(n * 2);

    for (size_t i = 0; i < n; ++i)
    {
        const auto& a = this->aabbs[i];
        for (size_t j = i + 1; j < n; ++j)
        {
            const auto& b = this->aabbs[j];

            if (a.maxX < b.minX) break;
            if (a.minY > b.maxY || b.minY > a.maxY || a.entity.id == b.entity.id) continue;

            outPairs.push_back({a.entity, b.entity});
        }
    }
    LOG_DEBUG("CollisionDetectionSystem: found {} collision pairs", outPairs.size());
}
