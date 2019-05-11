#include "Server.h"
#include <iostream>

bool Server::Initialize(IPEndpoint ip)
{
	master_fd.clear();
	connections.clear();

	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;

		listeningSocket = Socket(ip.GetIPVersion());
		if (listeningSocket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (listeningSocket.Listen(ip) == PResult::P_Success)
			{
				WSAPOLLFD listeningSocketFD = {};
				listeningSocketFD.fd = listeningSocket.GetHandle();
				listeningSocketFD.events = POLLRDNORM;
				listeningSocketFD.revents = 0;

				master_fd.push_back(listeningSocketFD);

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
	std::vector<WSAPOLLFD> use_fd = master_fd;

	if (WSAPoll(use_fd.data(), use_fd.size(), 1) > 0)
	{
#pragma region listener
		WSAPOLLFD & listeningSocketFD = use_fd[0];
		if (listeningSocketFD.revents & POLLRDNORM)
		{
			Socket newConnectionSocket;
			IPEndpoint newConnectionEndpoint;
			if (listeningSocket.Accept(newConnectionSocket, &newConnectionEndpoint) == PResult::P_Success)
			{
				connections.emplace_back(TCPConnection(newConnectionSocket, newConnectionEndpoint));
				TCPConnection & acceptedConnection = connections[connections.size() - 1];
				std::cout << acceptedConnection.ToString() << " - New connection accepted." << std::endl;
				WSAPOLLFD newConnectionFD = {};
				newConnectionFD.fd = newConnectionSocket.GetHandle();
				newConnectionFD.events = POLLRDNORM;
				newConnectionFD.revents = 0;
				master_fd.push_back(newConnectionFD);
			}
			else
			{
				std::cerr << "Failed to accept new connection." << std::endl;
			}
		}
#pragma endregion Code specific to the listening socket

		for (int i = 1; i < use_fd.size(); i++)
		{
			int connectionIndex = i - 1;
			TCPConnection & connection = connections[connectionIndex];

			if (use_fd[i].revents & POLLERR) //If error occurred on this socket
			{
				std::cout << "Poll error occured on: " << connection.ToString() << "." << std::endl;
				master_fd.erase(master_fd.begin() + i);
				use_fd.erase(use_fd.begin() + i);
				connection.Close();
				connections.erase(connections.begin() + connectionIndex);
				i -= 1;
				continue;
			}

			if (use_fd[i].revents & POLLHUP) //If poll hangup occurred on this socket
			{
				std::cout << "Poll hangup occured on: " << connection.ToString() << "." << std::endl;
				master_fd.erase(master_fd.begin() + i);
				use_fd.erase(use_fd.begin() + i);
				connection.Close();
				connections.erase(connections.begin() + connectionIndex);
				i -= 1;
				continue;
			}

			if (use_fd[i].revents & POLLNVAL) //If invalid socket
			{
				std::cout << "Invalid socket used on: " << connection.ToString() << "." << std::endl;
				master_fd.erase(master_fd.begin() + i);
				use_fd.erase(use_fd.begin() + i);
				connection.Close();
				connections.erase(connections.begin() + connectionIndex);
				i -= 1;
				continue;
			}

			if (use_fd[i].revents & POLLRDNORM) //If normal data can be read without blocking
			{
				char buffer[PNet::g_MaxPacketSize];
				int bytesReceived = 0;
				bytesReceived = recv(use_fd[i].fd, buffer, PNet::g_MaxPacketSize, 0);

				if (bytesReceived == 0) //If connection was lost
				{
					std::cout << "[Recv==0] Connection lost: " << connection.ToString() << "." << std::endl;
					master_fd.erase(master_fd.begin() + i);
					use_fd.erase(use_fd.begin() + i);
					connection.Close();
					connections.erase(connections.begin() + connectionIndex);
					i -= 1;
					continue;
				}

				if (bytesReceived == SOCKET_ERROR) //If error occurred on socket
				{
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK)
					{
						std::cout << "[Recv<0] Connection lost: " << connection.ToString() << "." << std::endl;
						master_fd.erase(master_fd.begin() + i);
						use_fd.erase(use_fd.begin() + i);
						connection.Close();
						connections.erase(connections.begin() + connectionIndex);
						i -= 1;
						continue;
					}
				}

				if (bytesReceived > 0)
				{
					std::cout << connection.ToString() << " - Message Size: " << bytesReceived << "." << std::endl;
				}

			}

		}

	}

	
}
