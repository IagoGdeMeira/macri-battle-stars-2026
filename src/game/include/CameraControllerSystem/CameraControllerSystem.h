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
        AABB bounds = AABB{limits::lowest(), limits::max(), limits::lowest(), limits::max()};
        float epsilon = 0.001f, smoothFactor = 0.12f;
        Dimension2D viewSize {800.f, 600.f};
        bool applyZoomToSize = false;
        float minMargin = 50.f;
        bool favorBigPlayers = true;
    };

    CameraControllerSystem(Config&& cfg);
    void update(UpdateContext& ctx) override;

private:
    Camera2D& camera;
    Window& window;

    float minZoom, maxZoom, padding, verticalOffset, epsilon, smoothFactor;
    AABB bounds;
    Dimension2D viewSize;
    float minMargin;
    bool favorBigPlayers;

    float selectZoom(const AABB& playerBounds) const;
    Position computeClampedCameraPosition(const Position& center, float targetZoom);
};

#endif // camera_controller_system_h
