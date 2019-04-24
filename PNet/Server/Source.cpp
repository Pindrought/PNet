//Server Code [Tutorial 16] dev
//Author: Jacob Preston 2019-04-19

#include <PNet\IncludeMe.h>
#include <iostream>

using namespace PNet;

bool ProcessPacket(Packet & packet)
{
	switch (packet.GetPacketType())
	{
	case PacketType::ChatMessage:
	{
		std::string msg;
		packet >> msg;
		std::cout << "Chat message: " << msg << std::endl;
		break;
	}
	case PacketType::IntegerArray:
	{
		uint32_t arraySize = 0;
		packet >> arraySize;
		std::cout << "Integer Array Size: " << arraySize << std::endl;
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t elementValue = 0;
			packet >> elementValue;
			std::cout << "Integer[" << i << "] " << elementValue << std::endl;
		}
		break;
	}
	default:
		std::cout << "Unhandled packet type: " << packet.GetPacketType() << std::endl;
		return false;
	}
	return true;
}

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;

		Socket socket;
		if (socket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Success)
			{
				std::cout << "Socket successfully listening on port 4790." << std::endl;
				Socket newConnection;
				if (socket.Accept(newConnection) == PResult::P_Success)
				{
					std::cout << "New connection accepted." << std::endl;

					std::string string1, string2, string3;
					Packet packet;
					while (true)
					{
						PResult result = newConnection.Recv(packet);
						if (result != PResult::P_Success)
							break;

						if (!ProcessPacket(packet))
							break;
					}

					newConnection.Close();
				}
				else
				{
					std::cerr << "Failed to accept new connection." << std::endl;
				}
			}
			else
			{
				std::cerr << "Failed to listen on port 4790." << std::endl;
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create." << std::endl;
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}