#include "MyServer.h"
#include <iostream>

void MyServer::OnConnect(TCPConnection & newConnection)
{
	std::cout << newConnection.ToString() << " - New connection established... Awaiting for account credentials..." << std::endl;
}

void MyServer::OnDisconnect(TCPConnection & lostConnection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;
}

PacketProcessingResult MyServer::ProcessPacket(int connectionIndex, std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_ClientConnect:
	{
		std::string username;
		std::string userpassword;
		*packet >> username >> userpassword;
		std::cout << "User tried to connect with the following credentials [" << username << "] / [" << userpassword << "]" << std::endl;
		if (IsAccountValid(username, userpassword))
		{
			std::cout << "Account credentials are valid!" << std::endl;
		}
		else
		{
			std::cout << "Account credentials are bad! Aborting connection!" << std::endl;
			connections[connectionIndex].shutdownMode = true; //By setting shutdownmode to true, this connection will close once all outgoing packets are sent. No more incoming packets will be processed for this connection.
			std::shared_ptr<Packet> badAccountDataPacket = std::make_shared<Packet>(PacketType::PT_BadAccountData);
			connections[connectionIndex].pm_outgoing.Append(badAccountDataPacket);
			return PacketProcessingResult::ProcessSuccess;
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return PacketProcessingResult::ProcessFailure;
	}

	return PacketProcessingResult::ProcessSuccess;
}