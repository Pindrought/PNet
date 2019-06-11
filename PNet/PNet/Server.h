#pragma once

#include "TCPConnection.h"

namespace PNet
{
	class Server
	{
	public:
		bool Initialize(IPEndpoint ip);
		void Frame();
		bool ProcessPacket(std::shared_ptr<Packet> packet);
	private:
		Socket listeningSocket;
		std::vector<WSAPOLLFD> masterfd;
		std::vector<TCPConnection> connections;
	};
}