#include "Server.h"
#include <iostream>

bool Server::Initialize(IPEndpoint ip)
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;

		listeningSocket = Socket(ip.GetIPVersion());
		if (listeningSocket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (listeningSocket.Listen(ip) == PResult::P_Success)
			{
				std::cout << "Socket successfully listening." << std::endl;
				return true;
			}
			else
			{
				std::cerr << "Failed to listen on port 4790." << std::endl;
			}
			listeningSocket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create." << std::endl;
		}
	}
	return false;
}

void Server::Frame() 
{
	Socket newConnection;
	if (listeningSocket.Accept(newConnection) == PResult::P_Success)
	{
		std::cout << "New connection accepted." << std::endl;
		newConnection.Close();
	}
	else
	{
		std::cerr << "Failed to accept new connection." << std::endl;
	}
}
