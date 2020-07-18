#pragma once
#include <stdint.h>

namespace PNet
{
	enum PacketType : uint16_t
	{
		PT_Invalid,
		PT_ClientConnect, //Sent from client to server to try to connect
		PT_BadAccountData, //Sent from server to client when bad account data has been sent
	};
}