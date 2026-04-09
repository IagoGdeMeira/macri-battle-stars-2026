#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "../Camera2D/Camera2D.h"

#include "../../engine/include/System/System.h"
#include "../../engine/include/Window/Window.h"

class CameraControllerSystem : public System
{
public:
    CameraControllerSystem(Camera2D& camera, Window& window) :
        camera(camera), window(window) {}

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;

    float minZoom = 0.5f;
    float maxZoom = 2.0f;
    float padding = 50.0f;
};

#endif // camera_controller_system_h
