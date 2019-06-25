#pragma once
#include "TCPConnection.h"

using namespace PNet;

namespace PNet
{
	class Server
	{
	public:
		bool Initialize(IPEndpoint ip);
		void Frame();
	protected:
		virtual void OnConnect(TCPConnection & newConnection);
		virtual void OnDisconnect(TCPConnection & lostConnection, std::string reason);
		void CloseConnection(int connectionIndex, std::string reason);
		virtual bool ProcessPacket(std::shared_ptr<Packet> packet);

		Socket listeningSocket;
		std::vector<TCPConnection> connections;
		std::vector<WSAPOLLFD> master_fd;
		std::vector<WSAPOLLFD> use_fd;
	};
}