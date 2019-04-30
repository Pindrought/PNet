#pragma once

namespace PNet
{
	static const int g_MaxPacketSize = 8192;
	#ifndef _WIN32
	static const int INVALID_SOCKET = ~0;
	typedef int BOOL;
	static const int FALSE = 0;
	static const int TRUE = 1;
	static const int SOCKET_ERROR = -1;

	#ifndef WSAGetLastError()
			#define WSAGetLastError() errno
	#endif
	#ifndef closesocket
			#define closesocket(fd) close(fd)
	#endif

	#endif
}