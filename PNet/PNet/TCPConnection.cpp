#include "TCPConnection.h"

namespace PNet
{
	TCPConnection::TCPConnection(Socket socket, IPEndpoint endpoint)
		:socket(socket), endpoint(endpoint)
	{
		stringRepresentation = "[" + endpoint.GetIPString();
		stringRepresentation += ":" + std::to_string(endpoint.GetPort()) + "]";
	}

	void TCPConnection::Close()
	{
		socket.Close();
	}

	std::string TCPConnection::ToString()
	{
		return stringRepresentation;
	}
	void TCPConnection::SendPacket(std::shared_ptr<Packet> packet)
	{
		pm_incoming.Append(packet);
	}
	void TCPConnection::SendPacket(std::shared_ptr<TextPacket> packet)
	{
		if (packet->flaggedForSend)
		{
			tpm_outgoing.Append(packet);
		}
		else
		{
			packet->buffer += PNET_TEXT_ENDOFPACKET;
			packet->flaggedForSend = true;
		}
	}
}
