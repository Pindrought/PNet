//Server Code [Tutorial 14]
//Author: Jacob Preston 2019-04-18

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
			if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Success)
			{
				std::cout << "Socket successfully listening on port 4790." << std::endl;
				Socket newConnection;
				if (socket.Accept(newConnection) == PResult::P_Success)
				{
					std::cout << "New connection accepted." << std::endl;

					std::string string1, string2;
					Packet packet;
					while (true)
					{
						PResult result = newConnection.Recv(packet);
						if (result != PResult::P_Success)
							break;

						packet >> string1 >> string2;
						std::cout << string1 << std::endl;
						std::cout << string2 << std::endl;
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