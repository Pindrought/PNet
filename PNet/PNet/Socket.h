#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"

namespace PNet
{
	class Socket
	{
	public:
		Socket(	IPVersion ipversion = IPVersion::IPv4,
				SocketHandle handle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		SocketHandle GetHandle();
		IPVersion GetIPVersion();
	private:
		IPVersion ipversion = IPVersion::IPv4;
		SocketHandle handle = INVALID_SOCKET;
	};
}