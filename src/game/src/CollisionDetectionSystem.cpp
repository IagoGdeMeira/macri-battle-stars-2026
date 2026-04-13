#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>

void CollisionDetectionSystem::update(UpdateContext& ctx)
{
    std::vector<CollisionPair> pairs;
    this->detect(ctx, pairs);

    for (const auto& pair : pairs)
    { ctx.eventBus.emit<CollisionEvent>(CollisionEvent{ pair.a, pair.b }); }
}

long long CollisionDetectionSystem::hash(int x, int y)
{ return (static_cast<long long>(x) << 32) | static_cast<unsigned int>(y); }

unsigned long long CollisionDetectionSystem::hashPair(Entity a, Entity b)
{
    const unsigned long long low = std::min(a.id, b.id);
    const unsigned long long high = std::max(a.id, b.id);
    return (low << 32) | high;
}

void CollisionDetectionSystem::buildGrid(UpdateContext& ctx, Grid& grid, float cellSize)
{
    auto& components = ctx.world.components();

    auto rects = View<TransformComponent, RectangleColliderComponent>(components);
    for (auto [e, t, r] : rects)
    {
        const float left = t.x - r.width * 0.5f;
        const float right = t.x + r.width * 0.5f;
        const float top = t.y - r.height * 0.5f;
        const float bottom = t.y + r.height * 0.5f;

        const int minCellX = static_cast<int>(std::floor(left / cellSize));
        const int maxCellX = static_cast<int>(std::floor(right / cellSize));
        const int minCellY = static_cast<int>(std::floor(top / cellSize));
        const int maxCellY = static_cast<int>(std::floor(bottom / cellSize));

        for (int cellX = minCellX; cellX <= maxCellX; ++cellX)
        {
            for (int cellY = minCellY; cellY <= maxCellY; ++cellY)
            { grid[hash(cellX, cellY)].entities.push_back(e); }
        }
    }

    auto circles = View<TransformComponent, CircleColliderComponent>(components);
    for (auto [e, t, c] : circles)
    {
        const float left = t.x - c.radius;
        const float right = t.x + c.radius;
        const float top = t.y - c.radius;
        const float bottom = t.y + c.radius;

        const int minCellX = static_cast<int>(std::floor(left / cellSize));
        const int maxCellX = static_cast<int>(std::floor(right / cellSize));
        const int minCellY = static_cast<int>(std::floor(top / cellSize));
        const int maxCellY = static_cast<int>(std::floor(bottom / cellSize));

        for (int cellX = minCellX; cellX <= maxCellX; ++cellX)
        {
            for (int cellY = minCellY; cellY <= maxCellY; ++cellY)
            { grid[hash(cellX, cellY)].entities.push_back(e); }
        }
    }
}

void CollisionDetectionSystem::detect(UpdateContext& ctx, std::vector<CollisionPair>& pairs)
{
    Grid grid;
    PairSet checkedPairs;
    const float cellSize = 100.0f;

    this->buildGrid(ctx, grid, cellSize);

    for (auto& [key, cell] : grid)
    { this->detectInCell(ctx, cell, pairs, checkedPairs); }
}

void CollisionDetectionSystem::detectInCell(
    UpdateContext& ctx, const Cell& cell, std::vector<CollisionPair>& pairs, PairSet& checkedPairs
) {
    auto& entities = cell.entities;

    for (size_t i = 0; i < entities.size(); ++i)
    {
        for (size_t j = i + 1; j < entities.size(); ++j)
        { this->detectPair(ctx, pairs, { entities[i], entities[j] }, checkedPairs); }
    }
}

void CollisionDetectionSystem::detectPair(
    UpdateContext& ctx, std::vector<CollisionPair>& pairs, CollisionPair pair, PairSet& checkedPairs
) {
    auto& components = ctx.world.components();
    auto& [a, b] = pair;
    const auto pairKey = CollisionDetectionSystem::hashPair(a, b);

    if (checkedPairs.find(pairKey) != checkedPairs.end()) return;
    checkedPairs.insert(pairKey);

    const bool aRect = components.has<RectangleColliderComponent>(a);
    const bool bRect = components.has<RectangleColliderComponent>(b);
    const bool aCircle = components.has<CircleColliderComponent>(a);
    const bool bCircle = components.has<CircleColliderComponent>(b);

    const auto& ta = components.get<TransformComponent>(a);
    const auto& tb = components.get<TransformComponent>(b);

    if (aRect && bRect)
    {
        const auto& ra = components.get<RectangleColliderComponent>(a);
        const auto& rb = components.get<RectangleColliderComponent>(b);

        if (CollisionDetectionSystem::rectToRect({ ta, ra }, { tb, rb }))
        { pairs.push_back({ a, b }); }

        return;
    }

    if (aCircle && bCircle)
    {
        const auto& ca = components.get<CircleColliderComponent>(a);
        const auto& cb = components.get<CircleColliderComponent>(b);

        if (CollisionDetectionSystem::circleToCircle({ ta, ca }, { tb, cb }))
        { pairs.push_back({ a, b }); }

        return;
    }

    if (aRect && bCircle)
    {
        const auto& ra = components.get<RectangleColliderComponent>(a);
        const auto& cb = components.get<CircleColliderComponent>(b);

        if (CollisionDetectionSystem::rectToCircle({ ta, ra }, { tb, cb }))
        { pairs.push_back({ a, b }); }

        return;
    }

    if (aCircle && bRect)
    {
        const auto& ca = components.get<CircleColliderComponent>(a);
        const auto& rb = components.get<RectangleColliderComponent>(b);

        if (CollisionDetectionSystem::rectToCircle({ tb, rb }, { ta, ca }))
        { pairs.push_back({ a, b }); }
    }
}

bool CollisionDetectionSystem::rectToRect(RectParams a, RectParams b)
{
    float leftA = a.transform.x - a.collider.width * 0.5f;
    float rightA = a.transform.x + a.collider.width * 0.5f;
    float topA = a.transform.y - a.collider.height * 0.5f;
    float bottomA = a.transform.y + a.collider.height * 0.5f;

    float leftB = b.transform.x - b.collider.width * 0.5f;
    float rightB = b.transform.x + b.collider.width * 0.5f;
    float topB = b.transform.y - b.collider.height * 0.5f;
    float bottomB = b.transform.y + b.collider.height * 0.5f;

    if (leftA >= rightB || leftB >= rightA) return false;
    if (topA >= bottomB || topB >= bottomA) return false;
    return true;
}

bool CollisionDetectionSystem::circleToCircle(CircleParams a, CircleParams b)
{
    float dx = a.transform.x - b.transform.x;
    float dy = a.transform.y - b.transform.y;
    float distanceSq = dx * dx + dy * dy;
    float radiusSum = a.collider.radius + b.collider.radius;
    return distanceSq <= radiusSum * radiusSum;
}

bool CollisionDetectionSystem::rectToCircle(RectParams rect, CircleParams circle)
{
    float rectLeft = rect.transform.x - rect.collider.width * 0.5f;
    float rectRight = rect.transform.x + rect.collider.width * 0.5f;
    float rectTop = rect.transform.y - rect.collider.height * 0.5f;
    float rectBottom = rect.transform.y + rect.collider.height * 0.5f;

    float closestX = std::max(rectLeft, std::min(circle.transform.x, rectRight));
    float closestY = std::max(rectTop, std::min(circle.transform.y, rectBottom));

    float dx = circle.transform.x - closestX;
    float dy = circle.transform.y - closestY;
    float distanceSq = dx * dx + dy * dy;

    return distanceSq <= circle.collider.radius * circle.collider.radius;
}
