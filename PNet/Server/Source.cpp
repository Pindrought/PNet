//Server Code [Tutorial 5] [Dev]
//Author: Jacob Preston 2019-04-05

#include <PNet\IncludeMe.h>
#include <iostream>

using namespace PNet;

int main()
{
	if (Network::Initialize())
	{
		IPEndpoint test("www.google.com", 8000);

		if (test.GetIPVersion() == IPVersion::IPv4)
		{
			std::cout << "Hostname: " << test.GetHostname() << std::endl;
			std::cout << "IP: " << test.GetIPString() << std::endl;
			std::cout << "IP Bytes..." << std::endl;
			std::vector<uint8_t> ipbytes = test.GetIPBytes();
			for (auto & digit : ipbytes)
			{
				std::cout << (int)digit << std::endl;
			}
		}


		std::cout << "Winsock api successfully initialized." << std::endl;
		Socket socket;
		if (socket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
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