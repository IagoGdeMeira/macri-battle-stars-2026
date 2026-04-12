#ifndef animation_h
#define animation_h

#include <vector>

struct AnimationFrame { int x, y, width, height; };

struct Animation
{
    std::vector<AnimationFrame> frames;

    float frameDuration = 0.1f;
    bool loop = true;
};

#endif // animation_h
