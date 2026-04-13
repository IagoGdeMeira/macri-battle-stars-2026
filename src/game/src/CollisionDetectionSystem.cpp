#include "../include/CollisionDetectionSystem/CollisionDetectionSystem.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

void CollisionDetectionSystem::update(UpdateContext& ctx)
{
    std::vector<CollisionPair> pairs;
    this->detect(ctx, pairs);

    for (const auto& pair : pairs)
    { ctx.eventBus.emit<CollisionEvent>(CollisionEvent{ pair.a, pair.b }); }
}

void CollisionDetectionSystem::detect(UpdateContext& ctx, std::vector<CollisionPair>& pairs)
{
    auto& components = ctx.world.components();

    auto rects = View<TransformComponent, RectangleColliderComponent>(components);
    auto circles = View<TransformComponent, CircleColliderComponent>(components);

    for (auto [a, ta, ra] : rects) for (auto [b, tb, rb] : rects)
    {
        if (a == b) continue;
        if (this->rectToRect({ ta, ra }, { tb, rb })) pairs.push_back({ Entity(a), Entity(b) });
    }

    for (auto [a, ta, ca] : circles) for (auto [b, tb, cb] : circles)
    {
        if (a == b) continue;
        if (this->circleToCircle({ ta, ca }, { tb, cb })) pairs.push_back({ Entity(a), Entity(b) });
    }

    for (auto [a, ta, ra] : rects) for (auto [b, tb, cb] : circles)
    {
        if (a == b) continue;
        if (this->rectToCircle({ ta, ra }, { tb, cb })) pairs.push_back({ Entity(a), Entity(b) });
    }
}

bool CollisionDetectionSystem::rectToRect(RectP a, RectP b)
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

bool CollisionDetectionSystem::circleToCircle(CircleP a, CircleP b)
{
    float dx = a.transform.x - b.transform.x;
    float dy = a.transform.y - b.transform.y;
    float distanceSq = dx * dx + dy * dy;
    float radiusSum = a.collider.radius + b.collider.radius;
    return distanceSq <= radiusSum * radiusSum;
}

bool CollisionDetectionSystem::rectToCircle(RectP rect, CircleP circle)
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
