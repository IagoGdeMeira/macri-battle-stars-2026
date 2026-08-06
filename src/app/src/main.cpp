#define SDL_MAIN_HANDLED

#include "Application/Application.h"

#include "domain/utils/Logger/Logger.h"

#include <exception>
#include <iostream>

int main()
{
    Logger::setLevel(Logger::LogLevel::ERROR);
    try
    {
        Application app;
        app.setWindowTitle("Macri Battle Stars").setWindowSize(800, 600);

        return app.run();
    }
    catch (const std::exception& e) { LOG_ERROR("Exception: {}", e.what()); }
    catch (...) { LOG_ERROR("Unknown exception occurred."); }
    return 1;
}
