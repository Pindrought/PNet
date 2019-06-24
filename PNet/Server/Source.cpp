//Server Code [Tutorial 8 dev] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-06-23

#include "MyServer.h"
#include <iostream>

using namespace PNet;

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