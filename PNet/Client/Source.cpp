//Client Code [Tutorial 10] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-06-28

#include "MyClient.h"
#include <iostream>

int main()
{
	if (Network::Initialize())
	{
		MyClient client;
		if (client.Connect(IPEndpoint("::1", 6112)))
		{
			while (client.IsConnected())
			{
				client.Frame();
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}