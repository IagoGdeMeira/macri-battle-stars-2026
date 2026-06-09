#ifndef stub_window_h
#define stub_window_h

#include "../../src/engine/include/Window/Window.h"

class StubWindow : public Window
{
public:
    void create(int w, int h, const char*) override { this->width = w; this->height = h; }
    void setResolution(int w, int h) override { this->width = w; this->height = h; }
    void setFullscreen(bool enabled) override { this->fullscreen = enabled; }
    void getSize(int& w, int& h) override { w = this->width; h = this->height; }

private:
    int width = 800, height = 600;
    bool fullscreen = false;
};

#endif // stub_window_h
