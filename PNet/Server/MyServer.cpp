#include "MyServer.h"
#include <iostream>

void MyServer::OnConnect(TCPConnection & newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;

	std::shared_ptr<Packet> welcomeMessagePacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*welcomeMessagePacket << std::string("Welcome!");

	std::cout << "Appending welcome message to new connection." << std::endl;
	newConnection.pm_outgoing.Append(welcomeMessagePacket);

	std::shared_ptr<Packet> newUserMessagePacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*newUserMessagePacket << std::string("New user connected!");
	for (auto & connection : connections)
	{
		if (&connection == &newConnection)
			continue;

		connection.pm_outgoing.Append(newUserMessagePacket);
	}
}

void MyServer::OnDisconnect(TCPConnection & lostConnection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;

	std::shared_ptr<Packet> newUserMessagePacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*newUserMessagePacket << std::string("User disconnected!");
	for (auto & connection : connections)
	{
		if (&connection == &lostConnection)
			continue;

		connection.pm_outgoing.Append(newUserMessagePacket);
	}
}

bool MyServer::ProcessPacket(std::shared_ptr<Packet> packet)
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
