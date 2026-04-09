#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "../Camera2D/Camera2D.h"

#include "../../engine/include/System/System.h"

class CameraControllerSystem : public System
{
public:
    CameraControllerSystem(Camera2D& camera);

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;

    float minX = 0.f;
    float minY = 0.f;
    float maxX = 2000.f;
    float maxY = 1000.f;

    float minZoom = 0.5f;
    float maxZoom = 2.0f;
};

#endif // camera_controller_system_h
