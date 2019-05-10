//Client Code [Tutorial 2] [Nonblocking]
//Author: Jacob Preston 2019-05-05

#include "Client.h"
#include <iostream>

int main()
{
	Client client;
	if (client.Connect(IPEndpoint("::1", 6112)))
	{
		while (client.IsConnected())
		{
			client.Frame();
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}