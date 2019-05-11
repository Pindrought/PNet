#pragma once
#include "Socket.h"

namespace PNet
{
	enum PacketTask
	{
		ProcessPacketSize,
		ProcessPacketContents
	};
	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		void Close();
		std::string ToString();
		Socket socket;

		PacketTask task = PacketTask::ProcessPacketSize;
		int extractionOffset = 0;
		uint16_t packetSize = 0;
		char buffer[PNet::g_MaxPacketSize];
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation = "";
	};
}