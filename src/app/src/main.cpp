#define SDL_MAIN_HANDLED

#include "../include/Application/Application.h"

int main()
{
    Application app;
    app.setWindowTitle("Macri Battle Stars")
       .setWindowSize(800, 600);

    return app.run();
}
