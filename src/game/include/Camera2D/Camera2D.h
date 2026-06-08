#ifndef camera_2d_h
#define camera_2d_h

#include "../../domain/include/Geometry/Geometry.h"

class Camera2D
{
public:
    void setPosition(float x, float y) { this->pos = { x, y }; }
    void move(float dx, float dy) { this->pos += { dx, dy }; }

    void setZoom(float z) { this->zoom = z; }

    Position getPosition() const { return this->pos; }
    float getZoom() const { return this->zoom; }

private:
    Position pos = { 0.f, 0.f };
    float zoom = 1.f;
};

#endif // camera_2d_h
