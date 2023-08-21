#pragma once
#include "ITcpAgent.h"

//-----------------------------------------------
class EXPORT Server : public ITcpAgent
{
public:
	bool Initialize(dbb::IPEndPoint ip) override;
	bool Frame() override;
	void SendMsg(std::string&& msg) override { SendMsgToAll(std::move(msg)); }
	void SendMsg(std::vector<uint32_t>&& msg) override { SendMsgToAll(std::move(msg)); }
	bool IsConnected() override { return true; /*@todo is init*/ }
	
	void SendMsgToAll(std::string&& msg);
	void SendMsgToAll(std::vector<uint32_t>&& msg);

protected:
	void CloseConnection(int connectionIndex, std::string reason);
	dbb::Socket listening_socket;

	std::vector<dbb::TCPConnection> connections;
	std::vector<WSAPOLLFD> master_fd;
	std::vector<WSAPOLLFD> use_fd;
};
