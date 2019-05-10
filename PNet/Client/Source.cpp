//Client Code NONBLOCKING DEV
//Author: Jacob Preston 2019-05-10

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