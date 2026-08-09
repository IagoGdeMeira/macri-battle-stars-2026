#ifndef animation_h
#define animation_h

#include <vector>

struct Animation
{
    struct Frame
    {
        int x, y, width, height;
        float duration = 0.f;

        bool operator==(const Frame& other) const
        {
            if (this == &other) return true;
            if (this->x != other.x) return false;
            if (this->y != other.y) return false;
            if (this->width != other.width) return false;
            if (this->height != other.height) return false;
            if (this->duration != other.duration) return false;
            return true;
        }
        bool operator!=(const Frame& other) const { return !(*this == other); }
    };

    std::vector<Frame> frames;

    float frameDuration = 0.1f;
    bool loop = true;

    bool operator==(const Animation& other) const
    {
        if (this == &other) return true;
        if (this->frames != other.frames) return false;
        if (this->frameDuration != other.frameDuration) return false;
        if (this->loop != other.loop) return false;
        return true;
    }
    bool operator!=(const Animation& other) const { return !(*this == other); }
};

#endif // animation_h
