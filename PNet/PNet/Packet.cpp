#include "Packet.h"

namespace PNet
{
	void Packet::Clear()
	{
		buffer.clear();
		extractionOffset = 0;
	}

	void Packet::Append(const void * data, uint32_t size)
	{
		buffer.insert(buffer.end(), (unsigned char*)data, (unsigned char*)data + size);
	}
	Packet & Packet::operator<<(uint32_t data)
	{
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}
	Packet & Packet::operator>>(uint32_t & data)
	{
		data = *reinterpret_cast<const uint32_t*>(&buffer[extractionOffset]);
		extractionOffset += sizeof(uint32_t);
		data = ntohl(data);
		return *this;
	}
	Packet & Packet::operator<<(const std::string & data)
	{
		*this << (uint32_t)data.size();
		Append(data.c_str(), data.size());
		return *this;
	}
	Packet & Packet::operator>>(std::string & data)
	{
		data.clear();
		uint32_t stringSize = 0;
		*this >> stringSize;

		data.resize(stringSize);
		data.assign((char*)&buffer[extractionOffset], stringSize);
		extractionOffset += stringSize;
		return *this;
	}
}
