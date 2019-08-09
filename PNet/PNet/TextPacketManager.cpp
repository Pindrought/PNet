#include "TextPacketManager.h"

namespace PNet
{
	void TextPacketManager::Clear()
	{
		packets = std::queue<std::shared_ptr<TextPacket>>{};//Clear out packet queue
	}

	bool TextPacketManager::HasPendingPackets()
	{
		return (!packets.empty()); //returns true if packets are pending
	}

	void TextPacketManager::Append(std::shared_ptr<TextPacket> p)
	{
		packets.push(std::move(p)); //Add packet to queue
	}

	std::shared_ptr<TextPacket> TextPacketManager::Retrieve()
	{
		std::shared_ptr<TextPacket> p = packets.front(); //Get packet from front of queue
		return p; //Return packet that was removed from the queue
	}

	void TextPacketManager::Pop()
	{
		packets.pop(); //Remove packet from front of queue
	}
}