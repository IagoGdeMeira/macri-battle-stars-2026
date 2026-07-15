#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>

void CollisionDetectionSystem::update(UpdateContext& ctx)
{
    Grid grid;
    this->buildGrid(ctx, grid);

    std::vector<ICollisionDetection::CollisionPair> pairs;
    this->collectPairs(grid, pairs);

    for (auto& detector : this->detectors) detector->detect(pairs, ctx);
}


unsigned long long CollisionDetectionSystem::hashPair(Entity a, Entity b)
{
    using ullong = unsigned long long;
    const ullong low = std::min(a.id, b.id);
    const ullong high = std::max(a.id, b.id);
    return (low << 32) | high;
}

void CollisionDetectionSystem::buildGrid(UpdateContext& ctx, Grid& grid)
{
    auto& comp = ctx.world.components();

    auto rects = View<TransformComponent, RectangleColliderComponent>(comp);
    for (auto [e, t, r] : rects)
    {
        auto& pos = t.position;
        const AABB rectBounds {
            pos.x - r.size.width * 0.5f, pos.x + r.size.width * 0.5f,
            pos.y - r.size.height * 0.5f, pos.y + r.size.height * 0.5f
        };
        const AABB cellBounds {
            std::floor(rectBounds.left / this->cellSize) * this->cellSize,
            std::floor(rectBounds.right / this->cellSize) * this->cellSize,
            std::floor(rectBounds.top / this->cellSize) * this->cellSize,
            std::floor(rectBounds.bottom / this->cellSize) * this->cellSize
        };

        for (int cellX = static_cast<int>(cellBounds.left); cellX <= static_cast<int>(cellBounds.right); ++cellX)
        {
            for (int cellY = static_cast<int>(cellBounds.top); cellY <= static_cast<int>(cellBounds.bottom); ++cellY)
            { grid[CollisionDetectionSystem::hash(cellX, cellY)].entities.push_back(e); }
        }
    }

    auto circles = View<TransformComponent, CircleColliderComponent>(comp);
    for (auto [e, t, c] : circles)
    {
        auto& pos = t.position;
        const AABB circleBounds{pos.x - c.radius, pos.x + c.radius, pos.y - c.radius, pos.y + c.radius};
        const AABB cellBounds{
            std::floor(circleBounds.left / this->cellSize) * this->cellSize,
            std::floor(circleBounds.right / this->cellSize) * this->cellSize,
            std::floor(circleBounds.top / this->cellSize) * this->cellSize,
            std::floor(circleBounds.bottom / this->cellSize) * this->cellSize};

        for (int cellX = static_cast<int>(cellBounds.left); cellX <= static_cast<int>(cellBounds.right); ++cellX)
        {
            for (int cellY = static_cast<int>(cellBounds.top); cellY <= static_cast<int>(cellBounds.bottom); ++cellY)
            { grid[CollisionDetectionSystem::hash(cellX, cellY)].entities.push_back(e); }
        }
    }
}

void CollisionDetectionSystem::collectPairs(Grid& grid, std::vector<ICollisionDetection::CollisionPair>& outPairs)
{
    PairSet checkedPairs;

    for (auto& [key, cell] : grid)
    {
        auto& entities = cell.entities;
        for (size_t i = 0; i < entities.size(); ++i) for (size_t j = i + 1; j < entities.size(); ++j)
        {
            Entity a = entities[i];
            Entity b = entities[j];
            unsigned long long pairKey = CollisionDetectionSystem::hashPair(a, b);
            if (checkedPairs.find(pairKey) != checkedPairs.end()) continue;
            checkedPairs.insert(pairKey);
            outPairs.push_back({a, b});
        }
    }
}
