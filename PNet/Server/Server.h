#pragma once

#include <PNet/IncludeMe.h>

using namespace PNet;

class Server
{
public:
	bool Initialize(IPEndpoint ip);
	void Frame();
private:
	Socket listeningSocket;
};