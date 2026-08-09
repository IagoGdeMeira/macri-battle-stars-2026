#include "CameraControllerSystem/CameraControllerSystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>

CameraControllerSystem::CameraControllerSystem(Config&& cfg) :
    camera(cfg.camera),
    window(cfg.window),
    minZoom(cfg.minZoom),
    maxZoom(cfg.maxZoom),
    padding(cfg.padding),
    verticalOffset(cfg.verticalOffset),
    bounds(cfg.bounds),
    epsilon(cfg.epsilon),
    smoothFactor(cfg.smoothFactor),
    viewSize(cfg.viewSize),
    minMargin(cfg.minMargin),
    favorBigPlayers(cfg.favorBigPlayers)
{ this->camera.setApplyZoomToSize(cfg.applyZoomToSize); }

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, PlayerComponent>(comp);

    size_t count = 0;
    float leftmost = limits::max(), rightmost = limits::lowest();
    float topmost = limits::max(), bottommost = limits::lowest();
    std::vector<AABB> individualBounds;

    for (auto [entity, transform, player] : view)
    {
        ++count;
        auto& pos = transform.position;

        Dimension2D halfSize {16.f, 16.f};
        if (comp.has<ChildrenComponent>(entity))
        {
            const auto& children = comp.get<ChildrenComponent>(entity).children;
            for (Entity child : children) if (comp.has<SpriteComponent>(child))
            {
                const auto& sprite = comp.get<SpriteComponent>(child);
                halfSize = {sprite.size.width * 0.5f, sprite.size.height * 0.5f};
                break;
            }
        }
        
        else if (comp.has<SpriteComponent>(entity))
        {
            const auto& sprite = comp.get<SpriteComponent>(entity);
            halfSize = {sprite.size.width * 0.5f, sprite.size.height * 0.5f};
        }

        float left   = pos.x - halfSize.width;
        float right  = pos.x + halfSize.width;
        float top    = pos.y - halfSize.height;
        float bottom = pos.y + halfSize.height;

        individualBounds.push_back({left, right, top, bottom});

        leftmost   = std::min(leftmost, left);
        rightmost  = std::max(rightmost, right);
        topmost    = std::min(topmost, top);
        bottommost = std::max(bottommost, bottom);
    }

    if (count == 0) return;

    AABB totalBounds = {leftmost, rightmost, topmost, bottommost};
    totalBounds.left   -= this->padding;
    totalBounds.right  += this->padding;
    totalBounds.top    -= this->padding;
    totalBounds.bottom += this->padding;

    float boxWidth  = std::max(totalBounds.right - totalBounds.left, 1.f);
    float boxHeight = std::max(totalBounds.bottom - totalBounds.top, 1.f);
    float zoomX = this->viewSize.width  / boxWidth;
    float zoomY = this->viewSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);
    float finalZoom  = std::clamp(targetZoom, this->minZoom, this->maxZoom);

    float geometricCenterX = (totalBounds.left + totalBounds.right) * 0.5f;
    float geometricCenterY = (totalBounds.top + totalBounds.bottom) * 0.5f;

    float halfWorldX = (this->viewSize.width  / finalZoom) * 0.5f;

    float minCenterX = limits::lowest();
    float maxCenterX = limits::max();
    for (const auto& b : individualBounds)
    {
        float minC = b.left + halfWorldX + this->minMargin;
        float maxC = b.right - halfWorldX - this->minMargin;
        minCenterX = std::max(minCenterX, minC);
        maxCenterX = std::min(maxCenterX, maxC);
    }

    float centerX;
    if (minCenterX > maxCenterX) centerX = geometricCenterX;
    else
    {
        float idealCenterX = geometricCenterX;
        centerX = std::clamp(idealCenterX, minCenterX, maxCenterX);
    }

    float centerY = geometricCenterY + this->verticalOffset;
    Position targetPos = this->computeClampedCameraPosition({centerX, centerY}, finalZoom);

    Position currentPos = this->camera.getPosition();
    float currentZoom = this->camera.getZoom();

    float newZoom = currentZoom + (finalZoom - currentZoom) * this->smoothFactor;
    Position newPos {
        currentPos.x + (targetPos.x - currentPos.x) * this->smoothFactor,
        currentPos.y + (targetPos.y - currentPos.y) * this->smoothFactor
    };

    if (std::abs(newZoom - currentZoom) > this->epsilon ||
        std::abs(newPos.x - currentPos.x) > this->epsilon ||
        std::abs(newPos.y - currentPos.y) > this->epsilon)
    {
        this->camera.setPosition(newPos.x, newPos.y);
        this->camera.setZoom(newZoom);
    }
}

float CameraControllerSystem::selectZoom(const AABB& playerBounds) const
{
    float boxWidth  = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);
    float zoomX = this->viewSize.width  / boxWidth;
    float zoomY = this->viewSize.height / boxHeight;
    return std::clamp(std::min(zoomX, zoomY), this->minZoom, this->maxZoom);
}

Position CameraControllerSystem::computeClampedCameraPosition(const Position& center, float targetZoom)
{
    Dimension2D halfScreen {(this->viewSize.width / targetZoom) * 0.5f, (this->viewSize.height / targetZoom) * 0.5f};
    float clampedX = center.x;
    float minClampX = this->bounds.left + halfScreen.width;
    float maxClampX = this->bounds.right - halfScreen.width;
    if (minClampX <= maxClampX) clampedX = std::clamp(center.x, minClampX, maxClampX);
    
    float clampedY = center.y;
    float minClampY = this->bounds.top + halfScreen.height;
    float maxClampY = this->bounds.bottom - halfScreen.height;
    if (minClampY <= maxClampY) clampedY = std::clamp(center.y, minClampY, maxClampY);

    return {clampedX, clampedY};
}
