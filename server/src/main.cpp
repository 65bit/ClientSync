#include "Server.hpp"

int main()
{
    Server server;

    if (server.init())
    {
        server.run();
    }

    return 0;
}