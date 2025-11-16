#include <iostream>
#include "Application.hpp"


int main(int argc, char* argv[])
{
    try
    {
        auto app{std::make_unique<Application>(argc, argv)};
        return app->run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Fatal error: Unknown\n";
        return 1;
    }
    return 0;
}
