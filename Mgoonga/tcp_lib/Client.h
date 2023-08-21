#pragma once
#include "ITcpAgent.h"

//-------------------------------------
class EXPORT Client : public ITcpAgent
{
public:
	bool Initialize(dbb::IPEndPoint ip) override { return Connect(ip); }
	bool Frame() override;
	void SendMsg(std::string&& msg) override;
	void SendMsg(std::vector<uint32_t>&& msg) override;
	bool IsConnected() override;
	
	bool Connect(dbb::IPEndPoint ip);
	void CloseConnection(const std::string& reason);

protected:
	bool is_connected = false;
	dbb::TCPConnection connection;
	WSAPOLLFD master_fd;
	WSAPOLLFD use_fd;
};
