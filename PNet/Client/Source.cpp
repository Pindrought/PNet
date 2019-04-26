//Client Code [Tutorial 17]
//Author: Jacob Preston 2019-04-26

#include <PNet\IncludeMe.h>
#include <iostream>

using namespace PNet;

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		Socket socket(IPVersion::IPv6);
		if (socket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (socket.Connect(IPEndpoint("::1", 4790)) == PResult::P_Success)
			{
				std::cout << "Successfully connected to server!" << std::endl;
				
				Packet stringPacket(PacketType::PT_ChatMessage);
				stringPacket << std::string("This is my string packet!");

				Packet integersPacket(PacketType::PT_IntegerArray);
				uint32_t arraySize = 6;
				uint32_t integerArray[6] = { 2, 5, 7, 1, 2, 9 };
				integersPacket << arraySize;
				for (auto integer : integerArray)
				{
					integersPacket << integer;
				}

				while (true)
				{
					PResult result;

					if (rand() % 2 == 0)
					{
						result = socket.Send(stringPacket);
					}
					else
					{
						result = socket.Send(integersPacket);
					}

					if (result != PResult::P_Success)
						break;

					std::cout << "Attempting to send chunk of data..." << std::endl;
					Sleep(500);
				}
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
	Network::Shutdown();
	system("pause");
	return 0;
}