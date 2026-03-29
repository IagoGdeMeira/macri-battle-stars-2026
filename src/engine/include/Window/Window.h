#ifndef window_h
#define window_h

#include <string>

class Window
{
public:
    virtual ~Window() = default;

    virtual void create(
        int width,
        int height,
        const std::string& title
    ) = 0;

    virtual void pollEvents() = 0;
    virtual bool shouldClose() const = 0;
};

#endif // window_h
