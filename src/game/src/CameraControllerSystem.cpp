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
    epsilon(cfg.epsilon),
    bounds(cfg.bounds),
    viewSize(cfg.viewSize)
{
    LOG_DEBUG("CameraControllerSystem initialized with viewSize=({},{})", viewSize.width, viewSize.height);
    this->camera.setApplyZoomToSize(cfg.applyZoomToSize);
}

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto playerBounds = this->computePlayerBounds(ctx);
    if (playerBounds.left > playerBounds.right || playerBounds.top > playerBounds.bottom) return;

    float boxWidth = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);

    float zoomX = this->viewSize.width / boxWidth;
    float zoomY = this->viewSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);

    float finalZoom = std::clamp(targetZoom, this->minZoom, this->maxZoom);

    Position center {
        (playerBounds.left + playerBounds.right) * 0.5f,
        (playerBounds.top + playerBounds.bottom) * 0.5f + this->verticalOffset
    };

    Position clampedPos = this->computeClampedCameraPosition(center, finalZoom);

    const float currentZoom = this->camera.getZoom();
    const Position currentPos = this->camera.getPosition();

    bool needsZoomUpdate =
        std::abs(finalZoom - currentZoom) > this->epsilon ||
        std::abs(clampedPos.x - currentPos.x) > this->epsilon ||
        std::abs(clampedPos.y - currentPos.y) > this->epsilon;

    if (needsZoomUpdate)
    {
        this->camera.setPosition(clampedPos.x, clampedPos.y);
        this->camera.setZoom(finalZoom);
    }

    LOG_DEBUG("CameraControllerSystem: center=({}, {}), targetZoom={}, finalZoom={}, boxSize=({},{}), padding={}",
        center.x, center.y, targetZoom, finalZoom,
        playerBounds.right - playerBounds.left,
        playerBounds.bottom - playerBounds.top,
        this->padding);
}

AABB CameraControllerSystem::computePlayerBounds(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    auto view = View<TransformComponent, PlayerComponent>(comp);
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

    return playerBounds;
}

float CameraControllerSystem::computeTargetZoom(const AABB& playerBounds)
{
    float boxWidth = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);

    float zoomX = this->viewSize.width / boxWidth;
    float zoomY = this->viewSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);

    LOG_DEBUG("viewSize=({},{}), box=({},{}), zoomX={}, zoomY={}, targetZoom={}",
        this->viewSize.width, this->viewSize.height, boxWidth, boxHeight, zoomX, zoomY, targetZoom);

    float clamped = std::clamp(targetZoom, this->minZoom, this->maxZoom);
    LOG_DEBUG("clamped zoom = {}", clamped);
    return clamped;
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
