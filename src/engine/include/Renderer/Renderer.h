#ifndef renderer_h
#define renderer_h

#include <string>

struct SpriteComponent;
struct TransformComponent;

class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual void clear() = 0;
    virtual void present() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;
};

#endif // renderer_h
