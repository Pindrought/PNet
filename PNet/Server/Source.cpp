//Server Code [Tutorial 8] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-06-24

#include "MyServer.h"
#include <iostream>

int main()
{
	MyServer server;
	if (server.Initialize(IPEndpoint("::", 6112)))
	{
		while (true)
		{
			server.Frame();
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}