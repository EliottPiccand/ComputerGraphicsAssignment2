#include <cstdlib>
#include <exception>
#include <print>

#include "Application.h"

int main()
{
    try
    {
        Application app;
        app.run();
    }
    catch (const std::exception &e)
    {
        std::println("an exception occured: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
