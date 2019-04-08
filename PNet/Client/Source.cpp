//Client Code [Tutorial 11]
//Author: Jacob Preston 2019-04-08

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
				char buffer[256];
				strcpy_s(buffer, "Hello world from client!\0");
				int bytesSent = 0;
				int result = PResult::P_Success;
				while (result == PResult::P_Success)
				{
					result = socket.Send(buffer, 256, bytesSent);
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