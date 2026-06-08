#ifndef camera_2d_h
#define camera_2d_h

class Camera2D
{
public:
    void setPosition(float x, float y) { this->x = x; this->y = y; }
    void move(float dx, float dy) { this->x += dx; this->y += dy; }

    void setZoom(float z) { this->zoom = z; }

    float getX() const { return this->x; }
    float getY() const { return this->y; }
    float getZoom() const { return this->zoom; }

private:
    float x = 0.f, y = 0.f, zoom = 1.f;
};

#endif // camera_2d_h
