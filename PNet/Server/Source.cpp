//Server Code [Account Example]
//Author: Jacob Preston 2020-07-18

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