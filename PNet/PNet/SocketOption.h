#pragma once
namespace PNet
{
	enum SocketOption
	{
		TCP_NoDelay, //TRUE = disable nagle's algorithm
		IPV6Only, //TRUE = only ipv6 connections can connect to ipv6 server, false = both ipv4 and ipv6 can connect
	};
}