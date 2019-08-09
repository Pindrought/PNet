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
		for (int i = 0; i < connections.size(); i++)
		{
			if (connections[i].pm_outgoing.HasPendingPackets())
			{
				master_fd[i + 1].events = POLLRDNORM | POLLWRNORM;
			}
		}

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
					WSAPOLLFD newConnectionFD = {};
					newConnectionFD.fd = newConnectionSocket.GetHandle();
					newConnectionFD.events = POLLRDNORM;
					newConnectionFD.revents = 0;
					master_fd.push_back(newConnectionFD);
					OnConnect(acceptedConnection);
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
								packet->buffer.resize(connection.pm_incoming.currentPacketSize + 2);
								memcpy(&packet->buffer[2], connection.buffer, connection.pm_incoming.currentPacketSize);

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
					if (connection.textTransmissionMode)
					{
						TextPacketManager & tpm = connection.tpm_outgoing;
						while (tpm.HasPendingPackets())
						{
							char * bufferPtr = &tpm.Retrieve()->buffer[0];
							tpm.currentPacketSize = tpm.Retrieve()->buffer.size();
							int bytesSent = send(use_fd[i].fd, (char*)(bufferPtr)+tpm.currentPacketExtractionOffset, tpm.currentPacketSize - tpm.currentPacketExtractionOffset, 0);
							if (bytesSent > 0)
							{
								tpm.currentPacketExtractionOffset += bytesSent;
							}

							if (tpm.currentPacketExtractionOffset == tpm.Retrieve()->buffer.size()) //If full packet size was sent
							{
								tpm.currentPacketExtractionOffset = 0;
								tpm.currentPacketSize = 0;
								tpm.Pop();
							}
							else //If full packet was not sent, break out of the loop for sending outgoing packets for this connection - we'll have to try again next time we are able to write normal data without blocking
							{
								break;
							}
						}
						if (!tpm.HasPendingPackets())
						{
							master_fd[i].events = POLLRDNORM;
						}
					}
					else
					{
						PacketManager & pm = connection.pm_outgoing;
						while (pm.HasPendingPackets())
						{
							char * bufferPtr = &pm.Retrieve()->buffer[0];
							pm.currentPacketSize = pm.Retrieve()->buffer.size();
							int bytesSent = send(use_fd[i].fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset, pm.currentPacketSize - pm.currentPacketExtractionOffset, 0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}

							if (pm.currentPacketExtractionOffset == pm.Retrieve()->buffer.size()) //If full packet size was sent
							{
								pm.currentPacketExtractionOffset = 0;
								pm.currentPacketSize = 0;
								pm.Pop();
							}
							else //If full packet was not sent, break out of the loop for sending outgoing packets for this connection - we'll have to try again next time we are able to write normal data without blocking
							{
								break;
							}
						}
						if (!pm.HasPendingPackets())
						{
							master_fd[i].events = POLLRDNORM;
						}
					}
				}

			}
		}

		for (int i = connections.size() - 1; i >= 0; i--)
		{
			if (connections[i].textTransmissionMode)
			{
				while (connections[i].tpm_incoming.HasPendingPackets())
				{
					std::shared_ptr<TextPacket> frontPacket = connections[i].tpm_incoming.Retrieve();
					if (!ProcessTextPacket(frontPacket))
					{
						CloseConnection(i, "Failed to process incoming packet.");
						break;
					}
					connections[i].tpm_incoming.Pop();
				}
			}
			else
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

	bool Server::ProcessPacket(std::shared_ptr<Packet> packet, int16_t connectionIndex)
	{
		if (packet->GetPacketType() == PacketType::PT_TextTransmissionMode)
		{
			connections[connectionIndex].textTransmissionMode = true;
			TextPacket confirmPacket("Ready");

		}
		std::cout << "Packet received with size: " << packet->buffer.size() << std::endl;
		return true;
	}

	bool ProcessTextPacket(std::shared_ptr<TextPacket> packet, int16_t connectionIndex)
	{
		std::vector<std::string> segments = packet->GetSegments();
		std::cout << "Text packet received with " << segments.size() << " segments." << std::endl;
		for (int i = 0; i < segments.size(); i++)
		{
			std::cout << "[" << i << "] :" << segments[i] << std::endl;
		}
		return true;
	}
}