#pragma once
#include "Socket.h"
#include "PacketManager.h"

namespace PNet
{
	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		TCPConnection() :socket(Socket()) {}
		void Close();
		std::string ToString();
		Socket socket;

		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		char buffer[PNet::g_MaxPacketSize];
		bool shutdownMode = false; //If shutdown mode is on, the connection will no longer process incoming packets. Once the outgoing packets are sent, the connection will be closed.
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation = "";
	};
}