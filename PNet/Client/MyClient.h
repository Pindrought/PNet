#pragma once
#include <PNet/IncludeMe.h>

class MyClient : public Client
{
public:
	void SendAccountCredentials(std::string username, std::string userpassword);
private:
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
	void OnConnect() override;
	void OnDisconnect(std::string reason) override;
	//void OnConnectFail() override;
};