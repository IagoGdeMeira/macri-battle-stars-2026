#ifndef camera_controller_system_h
#define camera_controller_system_h

#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/System/System.h"
#include "engine/include/Window/Window.h"
#include "engine/value_objects/Camera2D/Camera2D.h"

#include <limits>

class CameraControllerSystem : public System
{
public: 
    using limits = std::numeric_limits<float>;

    struct Config 
    {
        Camera2D& camera;
        Window& window;
        float minZoom = 0.8f, maxZoom = 2.f, padding = 50.f, verticalOffset = -0.f;
        bool applyZoomToSize = false;
        AABB bounds = AABB{limits::lowest(), limits::max(), limits::lowest(), limits::max()};
        float epsilon = 0.001f;
        Dimension2D viewSize{800.f, 600.f};
    };

    CameraControllerSystem(Config&& cfg);

    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;

    float minZoom, maxZoom, padding, verticalOffset, epsilon;
    AABB bounds;
    Dimension2D viewSize;

    AABB computePlayerBounds(UpdateContext& ctx);
    float computeTargetZoom(const AABB& playerBounds);
    Position computeClampedCameraPosition(const Position& center, float targetZoom);
};

#endif // camera_controller_system_h
