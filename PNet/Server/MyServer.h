#pragma once
#include <PNet/IncludeMe.h>

using namespace PNet;

class MyServer : public Server
{
	private:
		void OnConnect(TCPConnection & newConnection);
		void OnDisconnect(TCPConnection & lostConnection, std::string reason);
		bool ProcessPacket(std::shared_ptr<Packet> packet);
};