#ifndef window_h
#define window_h

class EventBus;

class Window
{
public:
    virtual ~Window() = default;

    virtual void create(
        int width,
        int height,
        const char* title
    ) = 0;

    virtual void setResolution(int width, int height) = 0;
    virtual void setFullscreen(bool enabled) = 0;

    virtual void getSize(int& width, int& height) = 0;
};

#endif // window_h
