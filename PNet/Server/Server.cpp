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
	use_fd = master_fd;

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

		for (int i = use_fd.size()-1; i >= 1; i--)
		{
			int connectionIndex = i - 1;
			TCPConnection & connection = connections[connectionIndex];

			if (use_fd[i].revents & POLLERR) //If error occurred on this socket
			{
				CloseConnection(connectionIndex, "POLLERR");
				continue;
			}

			if (use_fd[i].revents & POLLHUP) //If poll hangup occurred on this socket
			{
				CloseConnection(connectionIndex, "POLLHUP");
				continue;
			}

			if (use_fd[i].revents & POLLNVAL) //If invalid socket
			{
				CloseConnection(connectionIndex, "POLLNVAL");
				continue;
			}

			if (use_fd[i].revents & POLLRDNORM) //If normal data can be read without blocking
			{
				
				int bytesReceived = 0;

				if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
				{
					bytesReceived = recv(use_fd[i].fd, (char*)&connection.pm_incoming.currentPacketSize, sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset, 0); //attempt to read g_MaxPacketSize bytes
				}
				else
				{
					bytesReceived = recv(use_fd[i].fd, connection.buffer, connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset, 0); //attempt to read g_MaxPacketSize bytes
				}


				if (bytesReceived == 0) //If connection was lost
				{
					CloseConnection(connectionIndex, "Recv==0");
					continue;
				}

				if (bytesReceived == SOCKET_ERROR) //If error occurred on socket
				{
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK)
					{
						CloseConnection(connectionIndex, "Recv<0");
						continue;
					}
				}

				if (bytesReceived > 0)
				{
					connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
					if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
						{
							connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
							if (connection.pm_incoming.currentPacketSize > PNet::g_MaxPacketSize)
							{
								CloseConnection(connectionIndex, "Packet size too large.");
								continue;
							}
							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketContents;
						}
					}
					else //Process packet type
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
						{
							std::shared_ptr<Packet> receivedPacket = std::make_shared<Packet>();
							receivedPacket->buffer.resize(connection.pm_incoming.currentPacketSize);
							memcpy(&receivedPacket->buffer[0], connection.buffer, connection.pm_incoming.currentPacketSize);

							connection.pm_incoming.Append(receivedPacket);

							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentPacketSize = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketSize;
						}
					}

					std::cout << connection.ToString() << " - Message size: " << bytesReceived << std::endl;

				}
			}
		}
	}

	for (int i = connections.size() - 1; i >= 0; i--)
	{
		while (connections[i].pm_incoming.HasPendingPackets())
		{
			std::shared_ptr<Packet> frontPacket = connections[i].pm_incoming.Retrieve();
			if (!ProcessPacket(frontPacket))
			{
				CloseConnection(i, "Failed to process incoming packet.");
				break;
			}
			connections[i].pm_incoming.Pop();
		}
	}
	
}

void Server::CloseConnection(int connectionIndex, std::string reason)
{
	TCPConnection & connection = connections[connectionIndex];
	std::cout << "[" << reason << "] Connection lost: " << connection.ToString() << "." << std::endl;
	master_fd.erase(master_fd.begin() + (connectionIndex+1));
	use_fd.erase(use_fd.begin() + (connectionIndex + 1));
	connection.Close();
	connections.erase(connections.begin() + connectionIndex);
}

bool Server::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		*packet >> chatmessage;
		std::cout << "Chat Message: " << chatmessage << std::endl;
		break;
	}
	case PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		*packet >> arraySize;
		std::cout << "Array Size: " << arraySize << std::endl;
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t element = 0;
			*packet >> element;
			std::cout << "Element[" << i << "] - " << element << std::endl;
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}