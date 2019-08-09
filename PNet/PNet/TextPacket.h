#pragma once
#ifndef PNET_TEXT_SEPERATOR
#define PNET_TEXT_SEPERATOR '|'
#endif
#ifndef PNET_TEXT_ENDOFPACKET
#define PNET_TEXT_ENDOFPACKET '\n'
#endif
#include <vector>

namespace PNet
{
	class TextPacket
	{
		friend class Server;
		friend class TCPConnection;
	public:
		TextPacket(std::string data);
		void Append(std::string data);
		std::vector<std::string> GetSegments();
	private:
		std::string buffer = "";
		bool flaggedForSend = false;
	};
}