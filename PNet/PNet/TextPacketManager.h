#pragma once
#include <queue> //for std::queue
#include <memory> //for std::shared_ptr
#include "TextPacket.h"

namespace PNet
{
	class TextPacketManager
	{
	private:
		std::queue<std::shared_ptr<TextPacket>> packets;
	public:
		void Clear();
		bool HasPendingPackets();
		void Append(std::shared_ptr<TextPacket> p);
		std::shared_ptr<TextPacket> Retrieve();
		void Pop();

		uint16_t currentPacketSize = 0;
		int currentPacketExtractionOffset = 0;
	};
}