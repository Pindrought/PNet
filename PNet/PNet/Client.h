#pragma once
#include "TCPConnection.h"

using namespace PNet;

class Client
{
public:
	bool Connect(IPEndpoint ip);
	bool IsConnected();
	bool Frame();
protected:
	bool isConnected = false;
	TCPConnection connection;

	virtual void OnConnect();
	virtual void OnConnectFail();
	virtual void OnDisconnect(std::string reason);
	virtual bool ProcessPacket(std::shared_ptr<Packet> packet);

	void CloseConnection(std::string reason);
private:

	WSAPOLLFD master_fd;
	WSAPOLLFD use_fd;
};