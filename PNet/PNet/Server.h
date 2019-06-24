#pragma once
#include "TCPConnection.h"

using namespace PNet;

class Server
{
public:
	bool Initialize(IPEndpoint ip);
	void Frame();

protected:
	virtual void OnConnect(TCPConnection & connection);
	virtual void OnDisconnect(TCPConnection & connection, std::string reason);
	void CloseConnection(int connectionIndex, std::string reason);
	virtual bool ProcessPacket(std::shared_ptr<Packet> packet);
	Socket listeningSocket;
	std::vector<TCPConnection> connections;
	std::vector<WSAPOLLFD> master_fd;
	std::vector<WSAPOLLFD> use_fd;
};