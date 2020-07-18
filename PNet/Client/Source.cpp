//Client Code [Account Connection Example]
//Author: Jacob Preston 2020-07-18

#include "MyClient.h"
#include <iostream>

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Enter username:";
		std::string username;
		std::getline(std::cin, username);
		std::cout << "Enter password:";
		std::string userpassword;
		std::getline(std::cin, userpassword);

		MyClient client;
		if (client.Connect(IPEndpoint("::1", 6112)))
		{
			client.SendAccountCredentials(username, userpassword);
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