#define SDL_MAIN_HANDLED

#include "../include/Application/Application.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        Application app;
        app.setWindowTitle("Macri Battle Stars").setWindowSize(800, 600);

        return app.run();
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; }

    return 1;
}
