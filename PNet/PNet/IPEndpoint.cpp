#include "IPEndpoint.h"
#include <assert.h>
#include <iostream>

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

	IPEndpoint::IPEndpoint(sockaddr * addr)
	{
		assert(addr->sa_family == AF_INET);
		sockaddr_in * addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		ipversion = IPVersion::IPv4;
		port = ntohs(addrv4->sin_port);
		ip_bytes.resize(sizeof(ULONG));
		memcpy(&ip_bytes[0], &addrv4->sin_addr, sizeof(ULONG));
		ip_string.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &ip_string[0], 16);
		hostname = ip_string;
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
	sockaddr_in IPEndpoint::GetSockaddrIPv4()
	{
		assert(ipversion == IPVersion::IPv4);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
		addr.sin_port = htons(port);
		return addr;
	}
	void IPEndpoint::Print()
	{
		switch (ipversion)
		{
		case IPVersion::IPv4:
			std::cout << "IP Version: IPv4" << std::endl;
			break;
		case IPVersion::IPv6:
			std::cout << "IP Version: IPv6" << std::endl;
			break;
		default:
			std::cout << "IP Version: Unknown" << std::endl;
		}
		std::cout << "Hostname: " << hostname << std::endl;
		std::cout << "IP: " << ip_string << std::endl;
		std::cout << "Port: " << port << std::endl;
		std::cout << "IP bytes..." << std::endl;
		for (auto & digit : ip_bytes)
		{
			std::cout << (int)digit << std::endl;
		}
	}
}