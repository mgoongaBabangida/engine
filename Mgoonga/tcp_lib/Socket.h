#pragma once
#include "pch.h"
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <vector>
#include <string>
#include <optional>

struct sockaddr_in;

namespace dbb {

	uint32_t EXPORT Htonl(uint32_t);
	uint32_t EXPORT Ntohl(uint32_t);
	static const int g_max_packetSize = 8192;

	class Packet;

	enum class IPVersion
	{
		IPv4,
		IPv6
	};

	enum class PResult
	{
		SUCCESS,
		GENERIC_ERROR
	};

	struct EXPORT IPEndPoint
	{
		IPEndPoint() {}
		IPEndPoint(const char* ip, unsigned short port);
		IPEndPoint(sockaddr* addr);
		sockaddr_in GetSocketAddressIPv4() const;
		IPVersion m_version = IPVersion::IPv4;
		std::string m_hostname = "";
		std::string m_ip_string = "";
		std::vector<uint8_t> m_bytes;
		unsigned short m_port = 0;
	};

	//------------------------------------------------------------------
	class  EXPORT Socket
	{
		using SocketHandle = SOCKET;

	public:
		Socket(IPVersion versin = IPVersion::IPv4, SocketHandle handle = INVALID_SOCKET);
		PResult									Create();
		PResult									Close();
		PResult									Bind(const IPEndPoint&);
		PResult									Listen(const IPEndPoint&, uint32_t backlog = 5);
		PResult									Connect(const IPEndPoint&);
		std::optional<uint32_t> Send(const void*data, int bytesSent);
		std::optional<uint32_t> Recv(void* data, int bytesRecieved);
		PResult									SendAll(const void* data, int bytesSent);
		PResult									RecvAll(void* destination, int bytesRecieved);
		PResult									Send(Packet&);
		PResult									Recv(Packet&);
		std::optional<Socket>		Accept();
		SocketHandle						GetHandle() const;
		IPVersion								GetIPVersion() const;
		size_t									GetSocketError() const;
		PResult									SetBlocking(bool isblocking);
		IPEndPoint							GetAcceptedEndPoint() const;

	protected:
		SocketHandle	m_handle = INVALID_SOCKET;
		IPVersion			m_version = IPVersion::IPv4;
		size_t				m_lastError = 0;
		IPEndPoint		acceptedConnection;
	};
}
