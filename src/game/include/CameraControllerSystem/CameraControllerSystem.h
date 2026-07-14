#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "../Camera2D/Camera2D.h"

#include "../../domain/utils/Logger/Logger.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/System/System.h"
#include "../../engine/include/Window/Window.h"

#include <limits>

class CameraControllerSystem : public System
{
public: 
    using limits = std::numeric_limits<float>;

    struct Config 
    {
        Camera2D& camera;
        Window& window;
        float minZoom = 0.8f, maxZoom = 1.f, padding = 50.f;
        AABB bounds = AABB{limits::lowest(), limits::max(), limits::lowest(), limits::max()};
    };

    CameraControllerSystem(Config&& cfg) :
        camera(cfg.camera), window(cfg.window), minZoom(cfg.minZoom),
        maxZoom(cfg.maxZoom), padding(cfg.padding), bounds(cfg.bounds)
    { LOG_DEBUG("CameraControllerSystem constructed"); }

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;

    float minZoom, maxZoom, padding;
    AABB bounds;
};

#endif // camera_controller_system_h
