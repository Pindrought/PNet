#pragma once
#include <PNet/IncludeMe.h>

using namespace PNet;

class Client
{
public:
	bool Connect(IPEndpoint ip);
	bool IsConnected();
	bool Frame();
	bool ProcessPacket(Packet packet);
private:
	bool isConnected = false;
	Socket socket;
};