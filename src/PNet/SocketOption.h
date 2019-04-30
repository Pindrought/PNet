#pragma once
namespace PNet
{
	enum SocketOption
	{
		TCP_NoDelay, //TRUE = disable nagle's algorithm
		IPV6_Only, //TRUE = Only ipv6 can connect. FALSE = ipv4 and ipv6 can both connect.
	};
}