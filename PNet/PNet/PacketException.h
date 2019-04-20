#pragma once
#include <string>

namespace PNet
{
	class PacketException
	{
	public:
		PacketException(std::string exception)
			:exception(exception)
		{}
		const char * what()
		{
			return exception.c_str();
		}
		std::string ToString()
		{
			return exception;
		}
	private:
		std::string exception;
	};
}