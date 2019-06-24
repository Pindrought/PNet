//Client Code [Tutorial 8] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-06-24

#include "MyClient.h"
#include <iostream>


int main()
{
	MyClient client;
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