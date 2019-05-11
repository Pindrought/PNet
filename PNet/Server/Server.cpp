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
				std::cerr << "Failed to listen." << std::endl;
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
	WSAPOLLFD listeningSocketFD = {};
	listeningSocketFD.fd = listeningSocket.GetHandle();
	listeningSocketFD.events = POLLRDNORM;
	listeningSocketFD.revents = 0;

	if (WSAPoll(&listeningSocketFD, 1, 1) > 0)
	{
		if (listeningSocketFD.revents & POLLRDNORM)
		{
			Socket newConnectionSocket;
			IPEndpoint newConnectionEndpoint;
			if (listeningSocket.Accept(newConnectionSocket, &newConnectionEndpoint) == PResult::P_Success)
			{
				TCPConnection acceptedConnection(newConnectionSocket, newConnectionEndpoint);
				std::cout << acceptedConnection.ToString() << " - New connection accepted." << std::endl;
				acceptedConnection.Close();
			}
			else
			{
				std::cerr << "Failed to accept new connection." << std::endl;
			}
		}
	}

	
}
