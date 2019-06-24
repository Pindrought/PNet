#pragma once
#include <PNet/IncludeMe.h>

class MyClient : public Client
{
	bool ProcessPacket(std::shared_ptr<Packet> packet);
	void OnConnect();
};