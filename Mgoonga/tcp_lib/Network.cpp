#include "pch.h"
#include "Network.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

namespace dbb
{
	bool NetWork::Initialize()
	{
		WSADATA wsadata;
		int result = WSAStartup(MAKEWORD(2,2), &wsadata);
		if (result != 0)
			return false;
		if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
			return false;
		return true;
	}
	void NetWork::Shutdown()
	{
		WSACleanup();
	}
}
