#pragma once
#include "IPVersion.h"
#include <string>
#include <vector>
#include <stdint.h>

namespace PNet
{
	class IPEndpoint
	{
	public:
		IPEndpoint(const char * ip, unsigned short port);
		IPVersion GetIPVersion();
		std::vector<uint8_t> GetIPBytes();
		std::string GetHostname();
		std::string GetIPString();
		unsigned short GetPort();
	private:
		IPVersion ipversion = IPVersion::Unknown;
		std::vector<uint8_t> ip_bytes;
		std::string hostname = "";
		std::string ip_string = "";
		unsigned short port = 0;
	};
}