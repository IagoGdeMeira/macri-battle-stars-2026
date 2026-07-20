#ifndef camera_2d_h
#define camera_2d_h

#include "domain/value_objects/Geometry/Geometry.h"

class Camera2D
{
public:
    Position getPosition() const { return this->pos; }
    float getZoom() const { return this->zoom; }
    
    void setPosition(float x, float y) { this->pos = { x, y }; }
    void setZoom(float z) { this->zoom = z; }

    void move(float dx, float dy) { this->pos += { dx, dy }; }

    void setApplyZoomToSize(bool apply) { this->applyZoomToSize = apply; }
    bool getApplyZoomToSize() const { return this->applyZoomToSize; }

    float getSizeScale() const { return this->applyZoomToSize ? this->zoom : 1.f; }

private:
    Position pos = {0.f, 0.f};
    float zoom = 1.f;
    bool applyZoomToSize = false;
};

#endif // camera_2d_h
