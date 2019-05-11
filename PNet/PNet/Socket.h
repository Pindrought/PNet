#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"
#include "Constants.h"
#include "Packet.h"

namespace PNet
{
	class Socket
	{
	public:
		Socket(	IPVersion ipversion = IPVersion::IPv4,
				SocketHandle handle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		PResult Bind(IPEndpoint endpoint);
		PResult Listen(IPEndpoint endpoint, int backlog = 5);
		PResult Accept(Socket & outSocket, IPEndpoint * endpoint = nullptr);
		PResult Connect(IPEndpoint endpoint);
		PResult Send(const void * data, int numberOfBytes, int & bytesSent);
		PResult Recv(void * destination, int numberOfBytes, int & bytesReceived);
		PResult SendAll(const void * data, int numberOfBytes);
		PResult RecvAll(void * destination, int numberOfBytes);
		PResult Send(Packet & packet);
		PResult Recv(Packet & packet);
		SocketHandle GetHandle();
		IPVersion GetIPVersion();
		PResult SetBlocking(bool isBlocking);
	private:
		PResult SetSocketOption(SocketOption option, BOOL value);
		IPVersion ipversion = IPVersion::IPv4;
		SocketHandle handle = INVALID_SOCKET;
	};
}