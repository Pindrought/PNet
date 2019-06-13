#include "PacketManager.h"

namespace PNet
{
	void PacketManager::Clear()
	{
		packets = std::queue<std::shared_ptr<Packet>>{};//Clear out packet queue
	}

	bool PacketManager::HasPendingPackets()
	{
		return (!packets.empty()); //returns true if packets are pending
	}

	void PacketManager::Append(std::shared_ptr<Packet> p)
	{
		packets.push(std::move(p)); //Add packet to queue
	}

	std::shared_ptr<Packet> PacketManager::Retrieve()
	{
		std::shared_ptr<Packet> p = packets.front(); //Get packet from front of queue
		return p; //Return packet that was removed from the queue
	}

	void PacketManager::Pop()
	{
		packets.pop(); //Remove packet from front of queue
	}
}