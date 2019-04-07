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

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success)
		{
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
	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		default:
			return PResult::P_NotYetImplemented;
		}

		if (result != 0) //If an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}
}