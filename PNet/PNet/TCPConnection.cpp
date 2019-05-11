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
}
