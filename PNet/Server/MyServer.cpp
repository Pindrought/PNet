#include "MyServer.h"
#include <iostream>

void MyServer::OnConnect(TCPConnection & connection)
{
	std::cout << connection.ToString() << " - New connection accepted." << std::endl;

	std::shared_ptr<Packet> stringPacket = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*stringPacket << std::string("Welcome!");

	std::cout << "Appending welcome message to new connection." << std::endl;
	connection.pm_outgoing.Append(stringPacket);

}

void MyServer::OnDisconnect(TCPConnection & connection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << connection.ToString() << "." << std::endl;
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
