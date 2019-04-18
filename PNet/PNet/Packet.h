#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <winsock.h>

namespace PNet
{
	class Packet
	{
	public:
		void Clear();

		void Append(const void * data, uint32_t size);

		Packet& operator << (const uint32_t data);
		Packet& operator >> (uint32_t & data);

		Packet & operator << (const std::string & data);
		Packet & operator >> (std::string & data);


		uint32_t extractionOffset = 0;
		std::vector<unsigned char> buffer;
	private:

	};
}