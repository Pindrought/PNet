//Client Code [Tutorial 13]
//Author: Jacob Preston 2019-04-12

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
				
				std::string buffer = "Hello world from client!";

				while (true)
				{
					uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize); //convert from host to network byte order - all integers sent over a stream should be in network byte order
					int result = socket.SendAll(&bufferSize, sizeof(uint32_t));
					if (result != PResult::P_Success)
						break;

					result = socket.SendAll(buffer.data(), buffer.size());
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