#include "../include/CameraControllerSystem/CameraControllerSystem.h"

#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/Geometry/Geometry.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>
#include <limits>

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto view = View<TransformComponent, PlayerComponent>(ctx.world.components());

    auto it = view.begin();
    if (it == view.end()) return;

    AABB playerBounds =
    {
        std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()
    };

    for (; it != view.end(); ++it)
    {
        auto [entity, transform, player] = *it;

        auto& pos = transform.position;
        playerBounds.left = std::min(playerBounds.left, pos.x);
        playerBounds.top = std::min(playerBounds.top, pos.y);
        playerBounds.right = std::max(playerBounds.right, pos.x);
        playerBounds.bottom = std::max(playerBounds.bottom, pos.y);
    }

    Position center = {(playerBounds.left + playerBounds.right) * 0.5f, (playerBounds.top + playerBounds.bottom) * 0.5f};
    Dimension2D size {playerBounds.right - playerBounds.left, playerBounds.bottom - playerBounds.top};
    size = {std::max(size.width, 1.f), std::max(size.height, 1.f)};

    int screenW, screenH;
    this->window.getSize(screenW, screenH);

    Position zoom = { screenW / size.width, screenH / size.height };

    float targetZoom = std::min(zoom.x, zoom.y);
    targetZoom = std::clamp(targetZoom, minZoom, maxZoom);

    AABB map = this->bounds;
    Dimension2D halfScreen = {(screenW / targetZoom) * 0.5f, (screenH / targetZoom) * 0.5f};
    
    float clampedX = (map.left + map.right) * 0.5f;
    float minClampX = map.left + halfScreen.width;
    float maxClampX = map.right - halfScreen.width;
    if (minClampX <= maxClampX) clampedX = std::clamp(center.x, minClampX, maxClampX);

    float clampedY = (map.top + map.bottom) * 0.5f;
    float minClampY = map.top + halfScreen.height;
    float maxClampY = map.bottom - halfScreen.height;
    if (minClampY <= maxClampY) clampedY = std::clamp(center.y, minClampY, maxClampY);

    this->camera.setPosition(clampedX, clampedY);
    this->camera.setZoom(targetZoom);
}
