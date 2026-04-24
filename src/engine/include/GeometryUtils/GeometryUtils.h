#ifndef geometry_utils_h
#define geometry_utils_h

struct Position { float x = 0.0f, y = 0.0f; };

struct Circle { Position position; float radius = 0.0f; };

struct Rectangle { Position position; float width = 0.0f, height = 0.0f; };

#endif // geometry_utils_h
