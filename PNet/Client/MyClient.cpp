#include "MyClient.h"
#include <iostream>

void MyClient::SendAccountCredentials(std::string username, std::string userpassword)
{
	std::shared_ptr<Packet> accountPacket = std::make_shared<Packet>(PacketType::PT_ClientConnect);
	*accountPacket << username << userpassword;
	connection.pm_outgoing.Append(accountPacket);
}

bool MyClient::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_BadAccountData:
		std::cout << "Bad account data. Connection refused." << std::endl;
		return true;
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}

void MyClient::OnConnect()
{
	std::cout << "Connection with server established." << std::endl;
}

void MyClient::OnDisconnect(std::string reason)
{
	std::cout << "Lost connection to server! Reason: " << reason << std::endl;
}
