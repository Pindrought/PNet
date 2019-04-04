#include "Socket.h"
#include <assert.h>

namespace PNet
{
	Socket::Socket(IPVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle)
	{
		assert(ipversion == IPVersion::IPv4);
	}

	PResult Socket::Create()
	{
		assert(ipversion == IPVersion::IPv4);

		if (handle != INVALID_SOCKET)
		{
			return PResult::P_NotYetImplemented;
		}

		handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //attempt to create socket

		if (handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}

	PResult Socket::Close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);
		if (result != 0) //if error occurred while trying to close socket
		{
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	SocketHandle Socket::GetHandle()
	{
		return handle;
	}

	IPVersion Socket::GetIPVersion()
	{
		return ipversion;
	}
}