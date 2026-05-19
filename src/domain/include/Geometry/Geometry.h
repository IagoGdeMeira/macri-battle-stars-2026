#ifndef geometry_h
#define geometry_h

struct Position { float x = 0.f, y = 0.f; };

struct Dimension2D { float width = 0.f, height = 0.f; };

struct Circle { Position position; float radius = 0.f; };

struct Rectangle { Position position; Dimension2D size { 0.f, 0.f }; };

struct Corners { float topLeft = 0.f, topRight = 0.f, bottomRight = 0.f, bottomLeft = 0.f; };

struct AABB { float left = 0.f, right = 0.f, top = 0.f, bottom = 0.f; };

#endif // geometry_h
