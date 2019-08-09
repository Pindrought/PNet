//Server Code [Flexible] [Nonblocking] [Winsock]

#include "MyServer.h"
#include <iostream>

int main()
{
	std::shared_ptr<TextPacket> test = std::make_shared<TextPacket>("Data");
	test->Append("Other");
	test->Append("Okay");
	
	/*if (Network::Initialize())
	{
		MyServer server;
		if (server.Initialize(IPEndpoint("0.0.0.0", 6112)))
		{
			while (true)
			{
				server.Frame();
			}
		}
	}
	Network::Shutdown();*/
	system("pause");
	return 0;
}