//Client Code [Tutorial 4] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-05-11

#include "Client.h"
#include <iostream>

using namespace PNet;

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