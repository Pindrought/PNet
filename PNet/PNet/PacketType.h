#pragma once

namespace PNet
{
	enum PacketType : uint32_t
	{
		ChatMessage,
		IntegerArray,
		InvalidPacketType
	};
}