#include "IPEndpoint.h"
#include <WS2tcpip.h>

namespace PNet
{
	IPEndpoint::IPEndpoint(const char * ip, unsigned short port)
	{
		this->port = port;
		in_addr addr; //location to store ipv4 ip address
		int result = inet_pton(AF_INET, ip, &addr); //Attempt to convert ip from presentation to network format (string to 32 bit big endian integer)

		if (result == 1) //If conversion was successful
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				ip_string = ip; //If address was a valid ipv4 and not a hostname then ip_string = hostname
				hostname = ip;
				ip_bytes.resize(sizeof(ULONG));
				memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));
				ipversion = IPVersion::IPv4;
				return;
			}
		}
		else //If conversion failed, we will try to resolve an ipv4 hostname
		{
			addrinfo hints = {}; //hints are used to filter the results we get for getaddrinfo
			hints.ai_family = AF_INET; //ipv4 only
			addrinfo* hostinfo = nullptr;
			result = getaddrinfo(ip, NULL, &hints, &hostinfo);
			if (result == 0) //If the call to getaddrinfo was successful
			{
				if (hostinfo != nullptr)
				{
					sockaddr_in * host_sockaddr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);
					
					ip_string.resize(16); //Resize ip to store ipv4 address in text format
					inet_ntop(AF_INET, &host_sockaddr->sin_addr, &ip_string[0], 16); //Determine ip string from the integer representation of IPV4
					
					hostname = ip;

					ULONG ip_long = host_sockaddr->sin_addr.S_un.S_addr;
					ip_bytes.resize(sizeof(ULONG));
					memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG));

					ipversion = IPVersion::IPv4;

					freeaddrinfo(hostinfo); //memory cleanup from getaddrinfo call
					return;
				}
			}
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
