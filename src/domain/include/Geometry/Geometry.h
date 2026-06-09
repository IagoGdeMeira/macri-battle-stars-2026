#ifndef geometry_h
#define geometry_h

struct Position
{
    float x = 0.f, y = 0.f;

    Position operator+(const Position& other) const { return { this->x + other.x, this->y + other.y }; }
    Position operator-(const Position& other) const { return { this->x - other.x, this->y - other.y }; }
    Position operator*(float scalar) const { return { this->x * scalar, this->y * scalar }; }
    Position operator/(float scalar) const { return { this->x / scalar, this->y / scalar }; }

    Position& operator+=(const Position& other) { this->x += other.x; this->y += other.y; return *this; }
    Position& operator-=(const Position& other) { this->x -= other.x; this->y -= other.y; return *this; }
    Position& operator*=(float scalar) { this->x *= scalar; this->y *= scalar; return *this; }
    Position& operator/=(float scalar) { this->x /= scalar; this->y /= scalar; return *this; }
};

struct Dimension2D
{
    float width = 0.f, height = 0.f;

    Dimension2D operator+(const Dimension2D& other) const { return { this->width + other.width, this->height + other.height }; }
    Dimension2D operator-(const Dimension2D& other) const { return { this->width - other.width, this->height - other.height }; }
    Dimension2D operator*(float scalar) const { return { this->width * scalar, this->height * scalar }; }
    Dimension2D operator/(float scalar) const { return { this->width / scalar, this->height / scalar }; }

    Dimension2D& operator+=(const Dimension2D& other) { this->width += other.width; this->height += other.height; return *this; }
    Dimension2D& operator-=(const Dimension2D& other) { this->width -= other.width; this->height -= other.height; return *this; }
    Dimension2D& operator*=(float scalar) { this->width *= scalar; this->height *= scalar; return *this; }
    Dimension2D& operator/=(float scalar) { this->width /= scalar; this->height /= scalar; return *this; }  
};

struct Circle { Position position; float radius = 0.f; };

struct Rectangle { Position position; Dimension2D size { 0.f, 0.f }; };

struct Corners { float topLeft = 0.f, topRight = 0.f, bottomRight = 0.f, bottomLeft = 0.f; };

struct AABB { float left = 0.f, right = 0.f, top = 0.f, bottom = 0.f; };

#endif // geometry_h
