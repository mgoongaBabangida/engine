#pragma once
#include "TCPConnection.h"

//-------------------------------------------------
class EXPORT ITcpAgent
{
public:
  virtual bool Initialize(dbb::IPEndPoint ip) = 0;
  virtual bool Frame() = 0;
  virtual void SendMsg(std::string&& msg) = 0;
  virtual void SendMsg(std::vector<uint32_t>&& msg) = 0;
  virtual bool IsConnected() = 0;
  virtual ~ITcpAgent() {}

  Event<std::function<void(const dbb::TCPConnection&)>>	ConnectionEstablished;
};
