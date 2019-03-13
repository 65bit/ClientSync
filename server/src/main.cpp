#include "Server.hpp"

int main()
{
	Server server;

    if (server.init())
    {
        server.run();
		server.deinit();
    }

    return 0;
}