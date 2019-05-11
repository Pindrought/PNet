#pragma once

#include "Socket.h"

namespace PNet
{
	enum PacketTask
	{
		ProcessPacketSize,
		ProcessPacket
	};

	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		void Close();
		std::string ToString();
		Socket socket;

		PacketTask packet_task = PacketTask::ProcessPacketSize;
		uint16_t packet_size = 0;
		int extraction_offset = 0;
		char buffer[PNet::g_MaxPacketSize];

	private:
		IPEndpoint endpoint;
		std::string stringRepresentation="";
	};
}