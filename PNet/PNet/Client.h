#pragma once
#include "TCPConnection.h"

namespace PNet
{
	class Client
	{
	public:
		bool Connect(IPEndpoint ip);
		bool IsConnected();
		bool Frame();
	protected:
		virtual bool ProcessPacket(std::shared_ptr<Packet> packet);
		virtual void OnConnect();
		virtual void OnConnectFail();
		virtual void OnDisconnect(std::string reason);
		void CloseConnection(std::string reason);

		TCPConnection connection;
	private:
		bool isConnected = false;
		WSAPOLLFD master_fd;
		WSAPOLLFD use_fd;
	};
}