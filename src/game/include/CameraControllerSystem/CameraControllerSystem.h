#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "../Camera2D/Camera2D.h"

#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/System/System.h"
#include "../../engine/include/Window/Window.h"

#include <limits>

class CameraControllerSystem : public System
{
public: 
    CameraControllerSystem(Camera2D& camera, Window& window, AABB bounds = AABB{
        std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()
    }) : camera(camera), window(window), bounds(bounds) {}

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;
    AABB bounds;

    float minZoom = 0.5f;
    float maxZoom = 2.f;
    float padding = 50.f;
};

#endif // camera_controller_system_h
