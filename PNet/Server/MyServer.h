#pragma once
#include <PNet/IncludeMe.h>

using namespace PNet;

class MyServer : public Server
{
	private:
		void OnConnect(TCPConnection & connection);
		void OnDisconnect(TCPConnection & connection, std::string reason);
		bool ProcessPacket(std::shared_ptr<Packet> packet);
};