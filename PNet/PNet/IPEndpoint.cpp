#include "IPEndpoint.h"
#include <WS2tcpip.h>

namespace PNet
{
	IPEndpoint::IPEndpoint(const char * ip, unsigned short port)
	{
		this->port = port;

		in_addr addr; //location to store the ipv4 address
		int result = inet_pton(AF_INET, ip, &addr);

		if (result == 1)
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				ip_string = ip;
				hostname = ip;

				ip_bytes.resize(sizeof(ULONG));
				memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));

				ipversion = IPVersion::IPv4;
				return;
			}
		}

		//Attempt to resolve hostname to ipv4 address
		addrinfo hints = {}; //hints will filter the results we get back for getaddrinfo
		hints.ai_family = AF_INET; //ipv4 addresses only
		addrinfo * hostinfo = nullptr;
		result = getaddrinfo(ip, NULL, &hints, &hostinfo);
		if (result == 0)
		{
			sockaddr_in * host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

			ip_string.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);

			hostname = ip;

			ULONG ip_long = host_addr->sin_addr.S_un.S_addr; //get ip address as unsigned long
			ip_bytes.resize(sizeof(ULONG)); 
			memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG)); //copy bytes into our array of bytes representing ip address

			ipversion = IPVersion::IPv4;

			freeaddrinfo(hostinfo); //memory cleanup from getaddrinfo call
			return;
		}

	}

	IPVersion IPEndpoint::GetIPVersion()
	{
		return ipversion;
	}
	std::vector<uint8_t> IPEndpoint::GetIPBytes()
	{
		return ip_bytes;
	}
	std::string IPEndpoint::GetHostname()
	{
		return hostname;
	}
	std::string IPEndpoint::GetIPString()
	{
		return ip_string;
	}
	unsigned short IPEndpoint::GetPort()
	{
		return port;
	}
}