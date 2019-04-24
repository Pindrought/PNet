//Client Code [Tutorial 16] dev
//Author: Jacob Preston 2019-04-19

#include <PNet\IncludeMe.h>
#include <iostream>

using namespace PNet;

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		Socket socket;
		if (socket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (socket.Connect(IPEndpoint("127.0.0.1", 4790)) == PResult::P_Success)
			{
				std::cout << "Successfully connected to server!" << std::endl;
				

				Packet packet1(PacketType::ChatMessage);
				packet1 << std::string("This is the first string!");

				Packet packet2(PacketType::IntegerArray);
				uint32_t arraySize = 4;
				uint32_t intArray[4] = { 5, 7, 12, 3 };
				packet2 << arraySize;
				packet2 << intArray[0] << intArray[1] << intArray[2] << intArray[3];
				while (true)
				{

					PResult result;
					if (rand() % 2 == 0)
					{
						result = socket.Send(packet1);
					}
					else
					{
						result = socket.Send(packet2);
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