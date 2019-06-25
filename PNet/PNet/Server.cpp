#include "Server.h"
#include <iostream>
#include "Network.h"

namespace PNet
{
	bool Server::Initialize(IPEndpoint ip)
	{
		master_fd.clear();
		connections.clear();

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
					OnConnect(acceptedConnection);
					WSAPOLLFD newConnectionFD = {};
					newConnectionFD.fd = newConnectionSocket.GetHandle();
					newConnectionFD.events = POLLRDNORM | POLLWRNORM;
					newConnectionFD.revents = 0;
					master_fd.push_back(newConnectionFD);
				}
				else
				{
					std::cerr << "Failed to accept new connection." << std::endl;
				}
			}
#pragma endregion Code specific to the listening socket

			for (int i = use_fd.size() - 1; i >= 1; i--)
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
						bytesReceived = recv(use_fd[i].fd, (char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset, sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset, 0);
					}
					else //Process Packet Contents
					{
						bytesReceived = recv(use_fd[i].fd, (char*)&connection.buffer + connection.pm_incoming.currentPacketExtractionOffset, connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset, 0);
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
						else //Processing packet contents
						{
							if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
							{
								std::shared_ptr<Packet> packet = std::make_shared<Packet>();
								packet->buffer.resize(connection.pm_incoming.currentPacketSize);
								memcpy(&packet->buffer[0], connection.buffer, connection.pm_incoming.currentPacketSize);

								connection.pm_incoming.Append(packet);

								connection.pm_incoming.currentPacketSize = 0;
								connection.pm_incoming.currentPacketExtractionOffset = 0;
								connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketSize;
							}
						}
					}
				}

				if (use_fd[i].revents & POLLWRNORM) //If normal data can be written without blocking
				{
					PacketManager & pm = connection.pm_outgoing;
					while (pm.HasPendingPackets())
					{
						if (pm.currentTask == PacketManagerTask::ProcessPacketSize) //Sending packet size
						{
							pm.currentPacketSize = pm.Retrieve()->buffer.size();
							uint16_t bigEndianPacketSize = htons(pm.currentPacketSize);
							int bytesSent = send(use_fd[i].fd, (char*)(&bigEndianPacketSize) + pm.currentPacketExtractionOffset, sizeof(uint16_t) - pm.currentPacketExtractionOffset, 0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}

							if (pm.currentPacketExtractionOffset == sizeof(uint16_t)) //If full packet size was sent
							{
								pm.currentPacketExtractionOffset = 0;
								pm.currentTask = PacketManagerTask::ProcessPacketContents;
							}
							else //If full packet size was not sent, break out of the loop for sending outgoing packets for this connection - we'll have to try again next time we are able to write normal data without blocking
							{
								break;
							}
						}
						else //Sending packet contents
						{
							char * bufferPtr = &pm.Retrieve()->buffer[0];
							int bytesSent = send(use_fd[i].fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset, pm.currentPacketSize - pm.currentPacketExtractionOffset, 0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}

							if (pm.currentPacketExtractionOffset == pm.currentPacketSize) //If full packet contents have been sent
							{
								pm.currentPacketExtractionOffset = 0;
								pm.currentTask = PacketManagerTask::ProcessPacketSize;
								pm.Pop(); //Remove packet from queue after finished processing
							}
							else
							{
								break; //Added after tutorial was made 2019-06-24
							}
						}
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

	void Server::OnConnect(TCPConnection & newConnection)
	{
		std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;
	}

	void Server::OnDisconnect(TCPConnection & lostConnection, std::string reason)
	{
		std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;
	}

	void Server::CloseConnection(int connectionIndex, std::string reason)
	{
		TCPConnection & connection = connections[connectionIndex];
		OnDisconnect(connection, reason);
		master_fd.erase(master_fd.begin() + (connectionIndex + 1));
		use_fd.erase(use_fd.begin() + (connectionIndex + 1));
		connection.Close();
		connections.erase(connections.begin() + connectionIndex);
	}

	bool Server::ProcessPacket(std::shared_ptr<Packet> packet)
	{
		std::cout << "Packet received with size: " << packet->buffer.size() << std::endl;
		return true;
	}
}