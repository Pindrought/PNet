#pragma once
enum PacketProcessingResult
{
	ProcessFailure = 0,
	ProcessSuccess = 1,
	ProcessFailureConnectionClosed = 2,
};