#include "app.h"
#include <iostream>

Application::Application()
{
    std::cout << "Application initialized" << std::endl;
}

void Application::run()
{
    std::cout << "Hello, World! This is a C++ Console Application." << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
}

std::string Application::getVersion() const
{
    return "1.0.0";
}
