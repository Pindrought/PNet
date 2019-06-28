#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <winsock.h>
#include "PacketException.h"
#include "PacketType.h"

namespace PNet
{
	class Packet
	{
	public:
		Packet(PacketType packetType = PacketType::PT_Invalid);
		uint16_t GetPacketSize();
		PacketType GetPacketType();
		void AssignPacketSize(uint16_t size);
		void AssignPacketType(PacketType packetType);

		void Clear();
		void Append(const void * data, uint32_t size);

		Packet& operator << (uint32_t data);
		Packet& operator >> (uint32_t & data);

		Packet & operator << (const std::string & data);
		Packet & operator >> (std::string & data);

		uint32_t extractionOffset = 0;
		std::vector<char> buffer;
	};
}