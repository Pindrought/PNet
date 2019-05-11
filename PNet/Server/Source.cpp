//Server Code [Tutorial 1] [Nonblocking] [Winsock]
//Author: Jacob Preston 2019-05-11

#include "Server.h"
#include <iostream>

using namespace PNet;

//bool ProcessPacket(Packet & packet)
//{
//	switch (packet.GetPacketType())
//	{
//	case PacketType::PT_ChatMessage:
//	{
//		std::string chatmessage;
//		packet >> chatmessage;
//		std::cout << "Chat Message: " << chatmessage << std::endl;
//		break;
//	}
//	case PacketType::PT_IntegerArray:
//	{
//		uint32_t arraySize = 0;
//		packet >> arraySize;
//		std::cout << "Array Size: " << arraySize << std::endl;
//		for (uint32_t i = 0; i < arraySize; i++)
//		{
//			uint32_t element = 0;
//			packet >> element;
//			std::cout << "Element[" << i << "] - " << element << std::endl;
//		}
//		break;
//	}
//	default:
//		return false;
//	}
//
//	return true;
//}

int main()
{
	Server server;
	if (server.Initialize(IPEndpoint("::", 6112)))
	{
		while (true)
		{
			server.Frame();
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}