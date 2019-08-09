#include "TextPacket.h"
#include <sstream>
#include "PacketException.h"

std::vector<std::string> stringsplit(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}

namespace PNet
{
	TextPacket::TextPacket(std::string data)
	{
		buffer = data;
	}

	void TextPacket::Append(std::string data)
	{
		if (flaggedForSend)
			throw PacketException("TestPacket::Append(std::string) - Packet already flagged for send.");
		if (buffer == "")
		{
			buffer = data;
		}
		else
		{
			buffer = buffer + PNET_TEXT_SEPERATOR + data;
		}
	}

	std::vector<std::string> TextPacket::GetSegments()
	{
		return stringsplit(buffer, PNET_TEXT_SEPERATOR);
	}
}