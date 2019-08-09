#pragma once
#include <stdint.h>

namespace PNet
{
	enum PacketType : uint16_t
	{
		PT_Invalid,
		PT_TextTransmissionMode,
		PT_ChatMessage,
		PT_IntegerArray,
		PT_Test
	};
}