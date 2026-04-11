#include "../include/CameraControllerSystem/CameraControllerSystem.h"

#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <limits>

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto view = View<TransformComponent, PlayerComponent>(ctx.world.components());

    auto it = view.begin();
    if (it == view.end()) return;

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (; it != view.end(); ++it)
    {
        auto [entity, transform, player] = *it;

        minX = std::min(minX, transform.x);
        minY = std::min(minY, transform.y);
        maxX = std::max(maxX, transform.x);
        maxY = std::max(maxY, transform.y);
    }

    float centerX = (minX + maxX) * 0.5f;
    float centerY = (minY + maxY) * 0.5f;

    float width = (maxX - minX) + padding;
    float height = (maxY - minY) + padding;

    width = std::max(width, 1.0f);
    height = std::max(height, 1.0f);

    int screenW, screenH;
    this->window.getSize(screenW, screenH);

    float zoomX = static_cast<float>(screenW) / width;
    float zoomY = static_cast<float>(screenH) / height;

    float targetZoom = std::min(zoomX, zoomY);
    targetZoom = std::clamp(targetZoom, minZoom, maxZoom);

    float mapMinX = this->bounds.minX;
    float mapMaxX = this->bounds.maxX;
    float mapMinY = this->bounds.minY;
    float mapMaxY = this->bounds.maxY;

    float halfWidth = (screenW / targetZoom) * 0.5f;
    float halfHeight = (screenH / targetZoom) * 0.5f;

    float minClampX = mapMinX + halfWidth;
    float maxClampX = mapMaxX - halfWidth;

    float clampedX;
    if (minClampX > maxClampX) clampedX = (mapMinX + mapMaxX) * 0.5f;
    else clampedX = std::clamp(centerX, minClampX, maxClampX);

    float minClampY = mapMinY + halfHeight;
    float maxClampY = mapMaxY - halfHeight;

    float clampedY;
    if (minClampY > maxClampY) clampedY = (mapMinY + mapMaxY) * 0.5f;
    else clampedY = std::clamp(centerY, minClampY, maxClampY);

    camera.setPosition(clampedX, clampedY);
    camera.setZoom(targetZoom);
}
