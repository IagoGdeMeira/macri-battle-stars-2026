#ifndef animation_h
#define animation_h

#include <vector>

struct Animation
{
    struct Frame { int x, y, width, height; };

    std::vector<Frame> frames;

    float frameDuration = 0.1f;
    bool loop = true;
};

#endif // animation_h
