#include "Server.h"
#include <iostream>
#include "Network.h"

namespace PNet
{
	bool Server::Initialize(IPEndpoint ip)
	{
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
					std::cout << "Socket successfully listening." << std::endl;

					WSAPOLLFD listeningSocketFD = {};
					listeningSocketFD.fd = listeningSocket.GetHandle();
					listeningSocketFD.events = POLLRDNORM;
					listeningSocketFD.revents = 0;

					masterfd.push_back(listeningSocketFD);

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
		std::vector<WSAPOLLFD> use_fd = masterfd;

		if (WSAPoll(use_fd.data(), use_fd.size(), 1) > 0)
		{
			if (use_fd[0].revents & POLLRDNORM)
			{
				Socket newConnectionSocket;
				IPEndpoint newConnectionEndpoint;
				if (listeningSocket.Accept(newConnectionSocket, &newConnectionEndpoint) == PResult::P_Success)
				{
					std::cout << "New connection accepted." << std::endl;
					connections.emplace_back(TCPConnection(newConnectionSocket, newConnectionEndpoint));
					WSAPOLLFD newConnectionFD = {};
					newConnectionFD.fd = newConnectionSocket.GetHandle();
					newConnectionFD.events = POLLRDNORM | POLLWRNORM;
					newConnectionFD.revents = 0;
					masterfd.push_back(newConnectionFD);
				}
				else
				{
					std::cerr << "Failed to accept new connection." << std::endl;
				}
			}

			for (int i = use_fd.size()-1; i >= 1; i--)
			{
				const int connectionindex = (i - 1);

				if (use_fd[i].revents & POLLERR) //If error occurred on socket
				{
					std::cout << "Error occured on connection: " << connections[connectionindex].ToString() << std::endl;
					masterfd.erase(masterfd.begin() + i); //remove socket from master set
					use_fd.erase(use_fd.begin() + i); //remove socket from our current in use set
					connections[connectionindex].Close(); //close connection
					connections.erase(connections.begin() + connectionindex);
					continue; //skip to next for loop iteration
				}

				if (use_fd[i].revents & POLLHUP) //if connection was disconnected or aborted
				{
					std::cout << "Lost connection to: " << connections[connectionindex].ToString() << std::endl;
					masterfd.erase(masterfd.begin() + i); //remove socket from master set
					use_fd.erase(use_fd.begin() + i); //remove socket from our current in use set
					connections[connectionindex].Close(); //close connection
					connections.erase(connections.begin() + connectionindex);
					continue; //skip to next for loop iteration
				}

				if (use_fd[i].revents & POLLNVAL) //if invalid socket was used
				{
					//This should never happen, but we'll check for it anyways
					std::cout << "Connection closed due to invalid socket: " << connections[connectionindex].ToString() << std::endl;
					masterfd.erase(masterfd.begin() + i); //remove socket from master set
					use_fd.erase(use_fd.begin() + i); //remove socket from our current in use set
					connections[connectionindex].Close(); //close connection
					connections.erase(connections.begin() + connectionindex); //erase connection from our vector of connections
					continue; //skip to next for loop iteration
				}

				if (use_fd[i].revents & POLLRDNORM) //normal data may be read without blocking
				{
					//this is where we will recv data
					TCPConnection & connection = connections[connectionindex];
					int bytesReceived = 0;

					if (connection.packet_task == PacketTask::ProcessPacketSize)
					{
						bytesReceived = recv(use_fd[i].fd, (char*)&connection.packet_size, sizeof(uint16_t)-connection.extraction_offset, 0); //attempt to read g_MaxPacketSize bytes
					}
					else
					{
						bytesReceived = recv(use_fd[i].fd, connection.buffer, connection.packet_size - connection.extraction_offset, 0); //attempt to read g_MaxPacketSize bytes
					}

					if (bytesReceived == 0) //If connection was dropped
					{
						std::cout << "[Recv==0] Lost connection to: " << connections[connectionindex].ToString() << std::endl;
						masterfd.erase(masterfd.begin() + i); //remove socket from master set
						use_fd.erase(use_fd.begin() + i); //remove socket from our current in use set
						connections[connectionindex].Close();
						connections.erase(connections.begin() + connectionindex);
						continue; //skip to next for loop iteration
					}
					if (bytesReceived < 0) //If an error occurred
					{
						int error = WSAGetLastError(); //on linux, use errno
						if (error != WSAEWOULDBLOCK) //on linux, this would be (if errno != EWOULDBLOCK && errno != EAGAIN)
						{
							std::cout << "[Recv<0] Lost connection to: " << connections[connectionindex].ToString() << std::endl;
							masterfd.erase(masterfd.begin() + i); //remove socket from master set
							use_fd.erase(use_fd.begin() + i); //remove socket from our current in use set
							connections[connectionindex].Close();
							connections.erase(connections.begin() + connectionindex);
							continue; //skip to next for loop iteration
						}
					}
					if (bytesReceived > 0) //if we did receive data
					{
						connection.extraction_offset += bytesReceived;
						if (connection.packet_task == PacketTask::ProcessPacketSize)
						{
							if (connection.extraction_offset == sizeof(uint16_t))
							{
								connection.packet_size = ntohs(connection.packet_size);
								if (connection.packet_size > PNet::g_MaxPacketSize)
								{
									std::cout << "Connection closed due to invalid packet size: " << connection.packet_size << std::endl;
									connection.Close();
									continue;
								}
								connection.extraction_offset = 0;
								connection.packet_task = PacketTask::ProcessPacket;
							}
						}
						else //Process packet type
						{
							if (connection.extraction_offset == connection.packet_size)
							{
								Packet receivedPacket;
								receivedPacket.buffer.resize(connection.packet_size);
								memcpy(&receivedPacket.buffer[0], connection.buffer, connection.packet_size);
								
								if (!ProcessPacket(receivedPacket))
								{
									connection.Close();
									continue;
								}

								connection.extraction_offset = 0;
								connection.packet_size = 0;
								connection.packet_task = PacketTask::ProcessPacketSize;
							}
						}
						std::cout << connections[connectionindex].ToString() << " - Message size: " << bytesReceived << std::endl;
					}
				}

				if (use_fd[i].revents & POLLWRNORM) //normal data can be written without blocking
				{
					//this is where we will send data
				}
			}
		}
	}
	bool Server::ProcessPacket(Packet & packet)
	{
		switch (packet.GetPacketType())
		{
		case PacketType::PT_ChatMessage:
		{
			std::string chatmessage;
			packet >> chatmessage;
			std::cout << "Chat Message: " << chatmessage << std::endl;
			break;
		}
		case PacketType::PT_IntegerArray:
		{
			uint32_t arraySize = 0;
			packet >> arraySize;
			std::cout << "Array Size: " << arraySize << std::endl;
			for (uint32_t i = 0; i < arraySize; i++)
			{
				uint32_t element = 0;
				packet >> element;
				std::cout << "Element[" << i << "] - " << element << std::endl;
			}
			break;
		}
		default:
			std::cout << "Unknown packet type: " << packet.GetPacketType() << std::endl;
			return false;
		}

		return true;
	}
}