//Server Code [Tutorial 10] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-06-28

#include "MyServer.h"
#include <iostream>

int main()
{
	if (Network::Initialize())
	{
		MyServer server;
		if (server.Initialize(IPEndpoint("::", 6112)))
		{
			while (true)
			{
				server.Frame();
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}