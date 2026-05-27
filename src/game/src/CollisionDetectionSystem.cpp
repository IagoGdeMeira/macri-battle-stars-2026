#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/Geometry/Geometry.h"
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
        auto& pos = t.position;
        const AABB rectBounds
        {
            pos.x - r.size.width * 0.5f, pos.x + r.size.width * 0.5f,
            pos.y - r.size.height * 0.5f, pos.y + r.size.height * 0.5f
        };

        const AABB cellBounds
        {
            std::floor(rectBounds.left / cellSize) * cellSize,
            std::floor(rectBounds.right / cellSize) * cellSize,
            std::floor(rectBounds.top / cellSize) * cellSize,
            std::floor(rectBounds.bottom / cellSize) * cellSize
        };

        for (int cellX = static_cast<int>(cellBounds.left); cellX <= static_cast<int>(cellBounds.right); ++cellX)
        {
            for (int cellY = static_cast<int>(cellBounds.top); cellY <= static_cast<int>(cellBounds.bottom); ++cellY)
            { grid[CollisionDetectionSystem::hash(cellX, cellY)].entities.push_back(e); }
        }
    }

    auto circles = View<TransformComponent, CircleColliderComponent>(components);
    for (auto [e, t, c] : circles)
    {
        auto& pos = t.position;
        const AABB circleBounds { pos.x - c.radius, pos.x + c.radius, pos.y - c.radius, pos.y + c.radius };
        const AABB cellBounds
        {
            std::floor(circleBounds.left / cellSize) * cellSize,
            std::floor(circleBounds.right / cellSize) * cellSize,
            std::floor(circleBounds.top / cellSize) * cellSize,
            std::floor(circleBounds.bottom / cellSize) * cellSize
        };

        for (int cellX = static_cast<int>(cellBounds.left); cellX <= static_cast<int>(cellBounds.right); ++cellX)
        {
            for (int cellY = static_cast<int>(cellBounds.top); cellY <= static_cast<int>(cellBounds.bottom); ++cellY)
            { grid[CollisionDetectionSystem::hash(cellX, cellY)].entities.push_back(e); }
        }
    }
}

void CollisionDetectionSystem::detect(UpdateContext& ctx, std::vector<CollisionPair>& pairs)
{
    Grid grid;
    PairSet checkedPairs;
    const float cellSize = 100.f;

    this->buildGrid(ctx, grid, cellSize);

    for (auto& [key, cell] : grid)
    { this->detectInCell({ ctx, pairs }, cell, checkedPairs); }
}

void CollisionDetectionSystem::detectInCell(DetectionParams params, const Cell& cell, PairSet& checkedPairs)
{
    auto& entities = cell.entities;

    for (size_t i = 0; i < entities.size(); ++i)
    {
        for (size_t j = i + 1; j < entities.size(); ++j)
        { this->detectInPair(params, { entities[i], entities[j] }, checkedPairs); }
    }
}

void CollisionDetectionSystem::detectInPair(DetectionParams params, CollisionPair pair, PairSet& checkedPairs)
{
    auto& components = params.ctx.world.components();
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
        { params.pairs.push_back({ a, b }); }

        return;
    }

    if (aCircle && bCircle)
    {
        const auto& ca = components.get<CircleColliderComponent>(a);
        const auto& cb = components.get<CircleColliderComponent>(b);

        if (CollisionDetectionSystem::circleToCircle({ ta, ca }, { tb, cb }))
        { params.pairs.push_back({ a, b }); }

        return;
    }

    if (aRect && bCircle)
    {
        const auto& ra = components.get<RectangleColliderComponent>(a);
        const auto& cb = components.get<CircleColliderComponent>(b);

        if (CollisionDetectionSystem::rectToCircle({ ta, ra }, { tb, cb }))
        { params.pairs.push_back({ a, b }); }

        return;
    }

    if (aCircle && bRect)
    {
        const auto& ca = components.get<CircleColliderComponent>(a);
        const auto& rb = components.get<RectangleColliderComponent>(b);

        if (CollisionDetectionSystem::rectToCircle({ tb, rb }, { ta, ca }))
        { params.pairs.push_back({ a, b }); }
    }
}

bool CollisionDetectionSystem::rectToRect(RectParams a, RectParams b)
{
    auto& posA = a.transform.position;
    const AABB rectABounds =
    {
        posA.x - a.collider.size.width * 0.5f, posA.x + a.collider.size.width * 0.5f,
        posA.y - a.collider.size.height * 0.5f, posA.y + a.collider.size.height * 0.5f
    };

    auto& posB = b.transform.position;
    const AABB rectBBounds =
    {
        posB.x - b.collider.size.width * 0.5f, posB.x + b.collider.size.width * 0.5f,
        posB.y - b.collider.size.height * 0.5f, posB.y + b.collider.size.height * 0.5f
    };

    if (rectABounds.left >= rectBBounds.right || rectBBounds.left >= rectABounds.right) return false;
    if (rectABounds.top >= rectBBounds.bottom || rectBBounds.top >= rectABounds.bottom) return false;
    return true;
}

bool CollisionDetectionSystem::circleToCircle(CircleParams a, CircleParams b)
{
    auto& posA = a.transform.position;
    auto& posB = b.transform.position;
    Position delta { posB.x - posA.x, posB.y - posA.y };
    float distanceSq = delta.x * delta.x + delta.y * delta.y;

    float radiusSum = a.collider.radius + b.collider.radius;

    return distanceSq <= radiusSum * radiusSum;
}

bool CollisionDetectionSystem::rectToCircle(RectParams rect, CircleParams circle)
{
    auto& size = rect.collider.size;

    auto& posA = rect.transform.position;
    AABB rectBounds =
    {
        posA.x - size.width * 0.5f, posA.x + size.width * 0.5f,
        posA.y - size.height * 0.5f, posA.y + size.height * 0.5f
    };
    
    Position closest =
    {
        std::max(rectBounds.left, std::min(circle.transform.position.x, rectBounds.right)),
        std::max(rectBounds.top, std::min(circle.transform.position.y, rectBounds.bottom))
    };
    
    auto& posB = circle.transform.position;
    Position delta { posB.x - closest.x, posB.y - closest.y };
    float distanceSq = delta.x * delta.x + delta.y * delta.y;

    return distanceSq <= circle.collider.radius * circle.collider.radius;
}
