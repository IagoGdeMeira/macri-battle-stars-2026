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

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto playerBounds = this->computePlayerBounds(ctx);
    if (playerBounds.left > playerBounds.right || playerBounds.top > playerBounds.bottom) return;
    
    int winW = 0, winH = 0;
    this->window.getSize(winW, winH);
    Dimension2D screenSize = {static_cast<float>(winW), static_cast<float>(winH)};

    float targetZoom = this->computeTargetZoom(playerBounds, screenSize);

    Position center = {
        (playerBounds.left + playerBounds.right) * 0.5f,
        (playerBounds.top + playerBounds.bottom) * 0.5f + this->verticalOffset
    };
    Position clampedPos = this->computeClampedCameraPosition(center, targetZoom, screenSize);

    this->camera.setPosition(clampedPos.x, clampedPos.y);
    this->camera.setZoom(targetZoom);

    LOG_DEBUG("CameraControllerSystem: center=({}, {}), targetZoom={}, boxSize=({},{}), padding={}",
        center.x, center.y, targetZoom, playerBounds.right - playerBounds.left, playerBounds.bottom - playerBounds.top, this->padding);
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

float CameraControllerSystem::computeTargetZoom(const AABB& playerBounds, Dimension2D screenSize)
{
    float boxWidth = std::max(playerBounds.right - playerBounds.left, 1.f);
    float boxHeight = std::max(playerBounds.bottom - playerBounds.top, 1.f);

    float zoomX = screenSize.width / boxWidth;
    float zoomY = screenSize.height / boxHeight;
    float targetZoom = std::min(zoomX, zoomY);

    return std::clamp(targetZoom, this->minZoom, this->maxZoom);
}

Position CameraControllerSystem::computeClampedCameraPosition(const Position& center, float targetZoom, Dimension2D screenSize)
{
    Dimension2D halfScreen = {(screenSize.width / targetZoom) * 0.5f, (screenSize.height / targetZoom) * 0.5f};

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
