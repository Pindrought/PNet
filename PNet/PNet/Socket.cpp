#include "Socket.h"
#include <assert.h>
#include <iostream>

namespace PNet
{
	Socket::Socket(IPVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle)
	{
		assert(ipversion == IPVersion::IPv4 || ipversion == IPVersion::IPv6);
	}

	PResult Socket::Create()
	{
		assert(ipversion == IPVersion::IPv4 || ipversion == IPVersion::IPv6);

		if (handle != INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}

		handle = socket(ipversion == IPVersion::IPv4 ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP); //attempt to create socket

		if (handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}

		int result = closesocket(handle);
		if (result != 0) //if error occurred while trying to close socket
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	PResult Socket::Bind(IPEndpoint endpoint)
	{
		switch (endpoint.GetIPVersion())
		{
		case IPVersion::IPv4:
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			if (result != 0) //if an error occurred
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
			break;
		}
		case IPVersion::IPv6:
		{
			sockaddr_in6 addr6 = endpoint.GetSockaddrIPv6();
			int result = bind(handle, (sockaddr*)(&addr6), sizeof(sockaddr_in6));
			if (result != 0) //if an error occurred
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}
			break;
		}
		default:
			return PResult::P_GenericError;
		}
		

		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog)
	{
		if (ipversion == IPVersion::IPv6)
		{
			if (SetSocketOption(SocketOption::IPV6Only, FALSE) != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
		}

		if (Bind(endpoint) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}

		int result = listen(handle, backlog);
		if (result != 0) //If an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Accept(Socket & outSocket)
	{
		switch (ipversion)
		{
		case IPVersion::IPv4:
		{
			sockaddr_in addr = {};
			int len = sizeof(sockaddr_in);

			SocketHandle acceptedConnectionHandle = accept(handle, (sockaddr*)(&addr), &len);
			if (acceptedConnectionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}

			IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
			std::cout << "New IPv4 connection accepted!" << std::endl;
			newConnectionEndpoint.Print();
			outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
			break;
		}
		case IPVersion::IPv6:
		{
			sockaddr_in6 addr6 = {};
			int len = sizeof(sockaddr_in6);

			SocketHandle acceptedConnectionHandle = accept(handle, (sockaddr*)(&addr6), &len);
			if (acceptedConnectionHandle == INVALID_SOCKET)
			{
				int error = WSAGetLastError();
				return PResult::P_GenericError;
			}

			IPEndpoint newConnectionEndpoint((sockaddr*)&addr6);
			std::cout << "New IPv6 connection accepted!" << std::endl;
			newConnectionEndpoint.Print();
			outSocket = Socket(IPVersion::IPv6, acceptedConnectionHandle);
			break;
		}
		default:
			return PResult::P_GenericError;
		}
		
		return PResult::P_Success;
	}

	PResult Socket::Connect(IPEndpoint endpoint)
	{
		int result = 0;
		switch (endpoint.GetIPVersion())
		{
		case IPVersion::IPv4:
		{
			sockaddr_in addr = endpoint.GetSockaddrIPv4();
			result = connect(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			break;
		}
		case IPVersion::IPv6:
		{
			sockaddr_in6 addr6 = endpoint.GetSockaddrIPv6();
			result = connect(handle, (sockaddr*)(&addr6), sizeof(sockaddr_in6));
			break;
		}
		default:
			return PResult::P_GenericError;
		}
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(const void * data, int numberOfBytes, int & bytesSent)
	{
		bytesSent = send(handle, (const char*)data, numberOfBytes, NULL);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Recv(void * destination, int numberOfBytes, int & bytesReceived)
	{
		bytesReceived = recv(handle, (char*)destination, numberOfBytes, NULL);

		if (bytesReceived == 0) //If connection was gracefully closed
		{
			return PResult::P_GenericError;
		}

		if (bytesReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::SendAll(const void * data, int numberOfBytes)
	{

		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char * bufferOffset = (char*)data + totalBytesSent;
			PResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesSent += bytesSent;
		}

		return PResult::P_Success;
	}

	PResult Socket::RecvAll(void * destination, int numberOfBytes)
	{
		int totalBytesReceived = 0;

		while (totalBytesReceived < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesReceived;
			int bytesReceived = 0;
			char * bufferOffset = (char*)destination + totalBytesReceived;
			PResult result = Recv(bufferOffset, bytesRemaining, bytesReceived);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesReceived += bytesReceived;
		}

		return PResult::P_Success;
	}

	PResult Socket::Send(Packet & packet)
	{
		uint16_t encodedPacketSize = htons(packet.buffer.size());
		PResult result = SendAll(&encodedPacketSize, sizeof(uint16_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		result = SendAll(packet.buffer.data(), packet.buffer.size());
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	PResult Socket::Recv(Packet & packet)
	{
		packet.Clear();

		uint16_t encodedSize = 0;
		PResult result = RecvAll(&encodedSize, sizeof(uint16_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		uint16_t bufferSize = ntohs(encodedSize);

		if (bufferSize > PNet::g_MaxPacketSize)
			return PResult::P_GenericError;

		packet.buffer.resize(bufferSize);
		result = RecvAll(&packet.buffer[0], bufferSize);
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

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
		case SocketOption::IPV6Only:
		{
			if (ipversion == IPVersion::IPv4)
				return PResult::P_GenericError;
			result = setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value));
			break;
		}
		default:
			return PResult::P_GenericError;
		}

		if (result != 0) //If an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}
}