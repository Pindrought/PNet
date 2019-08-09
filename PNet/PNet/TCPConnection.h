#pragma once
#include "Socket.h"
#include "PacketManager.h"
#include "TextPacketManager.h"

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


		void SendPacket(std::shared_ptr<Packet> packet);
		void SendPacket(std::shared_ptr<TextPacket> packet);
		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		TextPacketManager tpm_incoming;
		TextPacketManager tpm_outgoing;
		char buffer[PNet::g_MaxPacketSize];
		bool textTransmissionMode = false; //simple transmission = text only (for dcs)
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation = "";
	};
}