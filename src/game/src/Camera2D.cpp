#include "../include/Camera2D/Camera2D.h"

void Camera2D::setPosition(float newX, float newY)
{
    this->x = newX;
    this->y = newY;
}

void Camera2D::move(float dx, float dy)
{
    this->x += dx;
    this->y += dy;
}
