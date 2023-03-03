#pragma once
#include "TCPConnection.h"

//-------------------------------------------------
class ITcpAgent
{
public:
  virtual bool Initialize(dbb::IPEndPoint ip) = 0;
  virtual bool Frame() =0;
  virtual void SendMsg(std::string&& msg) =0;
  virtual bool IsConnected() =0;
  virtual ~ITcpAgent() {}
};
