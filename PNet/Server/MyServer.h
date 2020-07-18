#pragma once
#include <PNet/IncludeMe.h>
#include "Account.h"

using namespace std;

class MyServer : public Server
{
private:
	void OnConnect(TCPConnection & newConnection) override;
	void OnDisconnect(TCPConnection & lostConnection, std::string reason) override;
	PacketProcessingResult ProcessPacket(int connectionIndex, std::shared_ptr<Packet> packet) override;
};