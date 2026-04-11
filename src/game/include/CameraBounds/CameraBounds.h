#ifndef camera_bounds_h
#define camera_bounds_h

#include <limits>

struct CameraBounds
{
    float minX = -std::numeric_limits<float>::infinity();
    float minY = -std::numeric_limits<float>::infinity();
    float maxX = std::numeric_limits<float>::infinity();
    float maxY = std::numeric_limits<float>::infinity();
};

#endif // camera_bounds_h
