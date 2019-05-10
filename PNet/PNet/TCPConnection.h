#pragma once

#include "Socket.h"

namespace PNet
{
	class TCPConnection
	{
	public:
		TCPConnection(Socket socket, IPEndpoint endpoint);
		void Close();
		std::string ToString();
		Socket socket;
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation="";
	};
}