#ifndef geometry_h
#define geometry_h

struct Position { float x = 0.0f, y = 0.0f; };

struct Circle { Position position; float radius = 0.0f; };

struct Rectangle { Position position; float width = 0.0f, height = 0.0f; };

struct Corners { float topLeft = 0.f, topRight = 0.f, bottomRight = 0.f, bottomLeft = 0.f; };

struct AABB { float left = 0.f, right = 0.f, top = 0.f, bottom = 0.f; };

#endif // geometry_h
