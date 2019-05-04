//Client Code [Tutorial 1] [Nonblocking]
//Author: Jacob Preston 2019-05-04

#include "Client.h"
#include <iostream>

int main()
{
	Client client;
	if (client.Connect(IPEndpoint("127.0.0.1", 6112)))
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