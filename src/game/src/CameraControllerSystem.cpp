#include "CameraControllerSystem/CameraControllerSystem.h"

#include "domain/components/PlayerComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"
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
    viewSize(cfg.viewSize)
{ this->camera.setApplyZoomToSize(cfg.applyZoomToSize); }

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto playerBounds = this->computePlayerBounds(ctx);
    if (playerBounds.left > playerBounds.right || playerBounds.top > playerBounds.bottom) return;

    float boxWidth      = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight     = std::max(playerBounds.bottom - playerBounds.top, 1.f);
    float targetZoom    = std::min(this->viewSize.width  / boxWidth, this->viewSize.height / boxHeight);
    float finalZoom     = std::clamp(targetZoom, this->minZoom, this->maxZoom);

    Position center {
        (playerBounds.left + playerBounds.right) * 0.5f,
        (playerBounds.top + playerBounds.bottom) * 0.5f + this->verticalOffset
    };

    Position targetPos = this->computeClampedCameraPosition(center, finalZoom);

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

AABB CameraControllerSystem::computePlayerBounds(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, PlayerComponent>(comp);
    
    size_t count = 0;
    for (auto [entity, transform, player] : view) { ++count; }
    LOG_DEBUG("CameraControllerSystem: {} players found", count);

    if (view.begin() == view.end()) return {limits::max(), limits::lowest(), limits::max(), limits::lowest()};

    AABB playerBounds = {limits::max(), limits::lowest(), limits::max(), limits::lowest()};
    for (auto [entity, transform, player] : view)
    {
        auto& pos = transform.position;
        Dimension2D halfSize {16.f, 16.f};
        if (comp.has<SpriteComponent>(entity))
        {
            const auto& sprite = comp.get<SpriteComponent>(entity);
            halfSize = {sprite.size.width * 0.5f, sprite.size.height * 0.5f};
        }
        playerBounds.left   = std::min(playerBounds.left, pos.x - halfSize.width);
        playerBounds.top    = std::min(playerBounds.top, pos.y - halfSize.height);
        playerBounds.right  = std::max(playerBounds.right, pos.x + halfSize.width);
        playerBounds.bottom = std::max(playerBounds.bottom, pos.y + halfSize.height);
    }

    playerBounds.left   -= this->padding;
    playerBounds.top    -= this->padding;
    playerBounds.right  += this->padding;
    playerBounds.bottom += this->padding;

    LOG_DEBUG("CameraControllerSystem: bounds left={}, right={}, top={}, bottom={}",
        playerBounds.left, playerBounds.right, playerBounds.top, playerBounds.bottom);

    return playerBounds;
}

float CameraControllerSystem::computeTargetZoom(const AABB& playerBounds)
{
    float boxWidth = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);

    float zoomX = this->viewSize.width / boxWidth;
    float zoomY = this->viewSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);

    return std::clamp(targetZoom, this->minZoom, this->maxZoom);
}

Position CameraControllerSystem::computeClampedCameraPosition(const Position& center, float targetZoom)
{
    Dimension2D halfScreen {
        (this->viewSize.width / targetZoom) * 0.5f,
        (this->viewSize.height / targetZoom) * 0.5f
    };

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
