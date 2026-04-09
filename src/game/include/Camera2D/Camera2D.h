#ifndef camera_2d_h
#define camera_2d_h

class Camera2D
{
public:
    void setPosition(float x, float y);
    void move(float dx, float dy);

    void setZoom(float z) { this->zoom = z; }

    float getX() const { return this->x; }
    float getY() const { return this->y; }
    float getZoom() const { return this->zoom; }

private:
    float x = 0.0f;
    float y = 0.0f;
    float zoom = 1.0f;
};

#endif // camera_2d_h
