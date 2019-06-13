#pragma once
#include <PNet/IncludeMe.h>

using namespace PNet;

class Server
{
	public:
		bool Initialize(IPEndpoint ip);
		void Frame();
	private:
		void CloseConnection(int connectionIndex, std::string reason);
		bool ProcessPacket(std::shared_ptr<Packet> packet);
		Socket listeningSocket;
		std::vector<TCPConnection> connections;
		std::vector<WSAPOLLFD> master_fd;
		std::vector<WSAPOLLFD> use_fd;
};