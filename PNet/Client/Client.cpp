#include "Client.h"
#include <iostream>

bool Client::Connect(IPEndpoint ip)
{
	isConnected = false;
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		socket = Socket(ip.GetIPVersion());
		if (socket.Create() == PResult::P_Success)
		{
			if (socket.SetBlocking(true) != PResult::P_Success)
				return false;


			std::cout << "Socket successfully created." << std::endl;
			if (socket.Connect(ip) == PResult::P_Success)
			{
				isConnected = true;
				std::cout << "Successfully connected to server!" << std::endl;
				return true;
			}
			else
			{
				std::cerr << "Failed to connect to server." << std::endl;
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create." << std::endl;
		}
	}
	return false;
}

bool Client::IsConnected()
{
	return isConnected;
}

bool Client::Frame()
{
	Packet incomingPacket;
	if (socket.Recv(incomingPacket) != PResult::P_Success)
	{
		std::cout << "Lost connection?" << std::endl;
		isConnected = false;
		return false;
	}

	if (!ProcessPacket(incomingPacket))
	{
		isConnected = false;
		return false;
	}

	return true;
}

bool Client::ProcessPacket(Packet & packet)
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
		std::cout << "Unrecognized packet type: " << packet.GetPacketType() << std::endl;
		return false;
	}

	return true;
}
