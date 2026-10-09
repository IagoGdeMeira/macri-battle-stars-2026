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

CameraControllerSystem::CameraControllerSystem(Config&& cfg) : cfg(std::move(cfg))
{ this->cfg.camera.setApplyZoomToSize(cfg.applyZoomToSize); }

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
    totalBounds.left   -= this->cfg.padding;
    totalBounds.right  += this->cfg.padding;
    totalBounds.top    -= this->cfg.padding;
    totalBounds.bottom += this->cfg.padding;

    float boxWidth  = std::max(totalBounds.right - totalBounds.left, 1.f);
    float boxHeight = std::max(totalBounds.bottom - totalBounds.top, 1.f);
    float zoomX = this->cfg.viewSize.width  / boxWidth;
    float zoomY = this->cfg.viewSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);
    float finalZoom  = std::clamp(targetZoom, this->cfg.minZoom, this->cfg.maxZoom);

    float geometricCenterX = (totalBounds.left + totalBounds.right) * 0.5f;
    float geometricCenterY = (totalBounds.top + totalBounds.bottom) * 0.5f;

    float halfWorldX = (this->cfg.viewSize.width  / finalZoom) * 0.5f;

    float minCenterX = limits::lowest();
    float maxCenterX = limits::max();
    for (const auto& b : individualBounds)
    {
        float minC = b.left + halfWorldX + this->cfg.minMargin;
        float maxC = b.right - halfWorldX - this->cfg.minMargin;
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

    float centerY = geometricCenterY + this->cfg.verticalOffset;
    Position targetPos = this->computeClampedCameraPosition({centerX, centerY}, finalZoom);

    Position currentPos = this->cfg.camera.getPosition();
    float currentZoom = this->cfg.camera.getZoom();

    float newZoom = currentZoom + (finalZoom - currentZoom) * this->cfg.smoothFactor;
    Position newPos {
        currentPos.x + (targetPos.x - currentPos.x) * this->cfg.smoothFactor,
        currentPos.y + (targetPos.y - currentPos.y) * this->cfg.smoothFactor
    };

    if (std::abs(newZoom - currentZoom) > this->cfg.epsilon ||
        std::abs(newPos.x - currentPos.x) > this->cfg.epsilon ||
        std::abs(newPos.y - currentPos.y) > this->cfg.epsilon)
    {
        this->cfg.camera.setPosition(newPos.x, newPos.y);
        this->cfg.camera.setZoom(newZoom);
    }
}

float CameraControllerSystem::selectZoom(const AABB& playerBounds) const
{
    float boxWidth  = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);
    float zoomX = this->cfg.viewSize.width  / boxWidth;
    float zoomY = this->cfg.viewSize.height / boxHeight;
    return std::clamp(std::min(zoomX, zoomY), this->cfg.minZoom, this->cfg.maxZoom);
}

Position CameraControllerSystem::computeClampedCameraPosition(const Position& center, float targetZoom)
{
    Dimension2D halfScreen {(this->cfg.viewSize.width / targetZoom) * 0.5f, (this->cfg.viewSize.height / targetZoom) * 0.5f};
    float clampedX = center.x;
    float minClampX = this->cfg.bounds.left + halfScreen.width;
    float maxClampX = this->cfg.bounds.right - halfScreen.width;
    if (minClampX <= maxClampX) clampedX = std::clamp(center.x, minClampX, maxClampX);
    
    float clampedY = center.y;
    float minClampY = this->cfg.bounds.top + halfScreen.height;
    float maxClampY = this->cfg.bounds.bottom - halfScreen.height;
    if (minClampY <= maxClampY) clampedY = std::clamp(center.y, minClampY, maxClampY);

    return {clampedX, clampedY};
}
