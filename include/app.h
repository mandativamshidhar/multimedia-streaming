#ifndef APP_H
#define APP_H

#include <string>

class Application
{
public:
    Application();
    void run();
    std::string getVersion() const;
};

#endif // APP_H
