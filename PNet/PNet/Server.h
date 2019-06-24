#pragma once
#include "TCPConnection.h"

using namespace PNet;

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

	std::vector<TCPConnection> connections;
private:
	Socket listeningSocket;
	std::vector<WSAPOLLFD> master_fd;
	std::vector<WSAPOLLFD> use_fd;
};