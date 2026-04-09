#include "../include/CameraControllerSystem/CameraControllerSystem.h"

#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>

void CameraControllerSystem::update(UpdateContext& ctx)
{
    auto view = View<TransformComponent, PlayerComponent>(ctx.world.components());

    float left = FLT_MAX;
    float right = -FLT_MAX;
    float top = FLT_MAX;
    float bottom = -FLT_MAX;

    for (auto [entity, transform, tag] : view)
    {
        left   = std::min(left, transform.x);
        right  = std::max(right, transform.x);
        top    = std::min(top, transform.y);
        bottom = std::max(bottom, transform.y);
    }

    float centerX = (left + right) * 0.5f;
    float centerY = (top + bottom) * 0.5f;

    camera.setPosition(centerX, centerY);

    float width = right - left;
    float height = bottom - top;

    float zoomX = 800.0f / width;
    float zoomY = 600.0f / height;

    float zoom = std::min(zoomX, zoomY);

    zoom = std::clamp(zoom, minZoom, maxZoom);

    camera.setZoom(zoom);
}
