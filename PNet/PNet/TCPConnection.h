#pragma once

#include "Socket.h"
#include "PacketManager.h"

namespace PNet
{
	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		void Close();
		std::string ToString();
		Socket socket;

		

		PacketManager pm_outgoing;
		PacketManager pm_incoming;
		char buffer[PNet::g_MaxPacketSize]; //Buffer will only be used for incoming packets to populate packets that are appended to incoming packet manager

	private:
		IPEndpoint endpoint;
		std::string stringRepresentation="";
	};
}