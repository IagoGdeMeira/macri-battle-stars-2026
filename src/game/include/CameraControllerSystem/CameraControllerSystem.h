#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "../Camera2D/Camera2D.h"
#include "../CameraBounds/CameraBounds.h"

#include "../../engine/include/System/System.h"
#include "../../engine/include/Window/Window.h"

#include <limits>

class CameraControllerSystem : public System
{
public:
    CameraControllerSystem(Camera2D& camera, Window& window) :
        camera(camera),
        window(window),
        bounds(CameraBounds{
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()
        }) {}

    CameraControllerSystem(Camera2D& camera, Window& window, CameraBounds bounds) :
        camera(camera), window(window), bounds(bounds) {}

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;
    CameraBounds bounds;

    float minZoom = 0.5f;
    float maxZoom = 2.0f;
    float padding = 50.0f;
};

#endif // camera_controller_system_h
