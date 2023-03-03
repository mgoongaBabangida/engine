#include "pch.h"
#include "Socket.h"
#include "Packet.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace dbb {

	uint32_t Htonl(uint32_t in)
	{
		return in;
	}
	uint32_t Ntohl(uint32_t in)
	{
		return in;
	}
	Socket::Socket(IPVersion versin, SocketHandle handle)
		: m_version(versin), m_handle(handle)
	{
	}

	PResult Socket::Create()
	{
		if (m_handle == INVALID_SOCKET)
			m_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_handle == INVALID_SOCKET)
			m_lastError = GetLastError();
		if (SetBlocking(false) != PResult::SUCCESS)
			return PResult::GENERIC_ERROR;

		BOOL result = 0;
		setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&result, sizeof(result));
		if (result != 0)
		{
			m_lastError = WSAGetLastError();
			return PResult::GENERIC_ERROR;
		}
		return PResult::SUCCESS;
	}

	PResult Socket::Close()
	{
		if(!closesocket(m_handle))
			return PResult::SUCCESS;
		else
			m_lastError = GetLastError();
		return PResult::GENERIC_ERROR;
	}
	PResult Socket::Bind(const IPEndPoint& endpoint)
	{
		sockaddr_in addr = endpoint.GetSocketAddressIPv4();
		if (!bind(m_handle,(sockaddr*)&addr,sizeof(sockaddr_in)))
			return PResult::SUCCESS;
		else
			m_lastError = GetLastError();
		return PResult::GENERIC_ERROR;
	}
	PResult Socket::Listen(const IPEndPoint& endpoint, uint32_t backlog)
	{
		if (Bind(endpoint) == PResult::SUCCESS && !listen(m_handle, backlog))
			return PResult::SUCCESS;
		else
			m_lastError = GetLastError();
		return PResult::GENERIC_ERROR;
	}
	PResult Socket::Connect(const IPEndPoint& server)
	{
		sockaddr_in addr = server.GetSocketAddressIPv4();
		if(!connect(m_handle, (sockaddr*)&addr, sizeof(sockaddr_in)))
			return PResult::SUCCESS;
		else
			m_lastError = WSAGetLastError();
		return PResult::GENERIC_ERROR;
	}
	std::optional<uint32_t> Socket::Send(const void* data, int bytesSent)
	{
		if (auto sent = send(m_handle, (const char*)data, bytesSent, NULL); sent != SOCKET_ERROR)
			return sent;
		else
			m_lastError = GetLastError();
		return std::nullopt;
	}
	std::optional<uint32_t> Socket::Recv(void* destination, int bytesSent)
	{
		if (auto recieved = recv(m_handle, (char*)destination, bytesSent, NULL); recieved != SOCKET_ERROR)
			return recieved;
		else
			m_lastError = GetLastError();
		return std::nullopt;
	}
	PResult Socket::SendAll(const void* data, int totalBytesSent)
	{
		int bytesSentAlready = 0;
		while (bytesSentAlready < totalBytesSent)
		{
			int bytesRemaining = totalBytesSent - bytesSentAlready;
			char* m_buffer = (char*)data + bytesSentAlready;
			if (auto sent = Send(m_buffer, bytesRemaining); sent.has_value())
				bytesSentAlready += sent.value();
			else
				return PResult::GENERIC_ERROR;
		}
		return PResult::SUCCESS;
	}
	PResult Socket::RecvAll(void* destination, int totalBytesRecieved)
	{
		int bytesRecievedAlready = 0;
		while (bytesRecievedAlready < totalBytesRecieved)
		{
			int bytesRemaining = totalBytesRecieved - bytesRecievedAlready;
			char* m_buffer = (char*)destination + bytesRecievedAlready;
			if (auto recv = Recv(m_buffer, bytesRemaining); recv.has_value())
				bytesRecievedAlready += recv.value();
			else
				return PResult::GENERIC_ERROR;
		}
		return PResult::SUCCESS;
	}
	PResult Socket::Send(Packet& packet)
	{
		uint16_t encodedPacketSize = packet.m_buffer.size(); //htons
		PResult result = SendAll(&encodedPacketSize, sizeof(uint16_t));
		if (result != PResult::SUCCESS)
			return PResult::GENERIC_ERROR;

		result = SendAll(packet.m_buffer.data(), packet.m_buffer.size());
		if (result != PResult::SUCCESS)
			return PResult::GENERIC_ERROR;

		return PResult::SUCCESS;
	}
	PResult Socket::Recv(Packet& packet)
	{
		packet.Clear();

		uint16_t encodedSize = 0;
		PResult result = RecvAll(&encodedSize, sizeof(uint16_t));
		if (result != PResult::SUCCESS)
			return PResult::GENERIC_ERROR;

		uint16_t bufferSize = encodedSize;//ntohs

		if (bufferSize > dbb::g_max_packetSize)
			return PResult::GENERIC_ERROR;

		packet.m_buffer.resize(bufferSize);
		result = RecvAll(&packet.m_buffer[0], bufferSize);
		if (result != PResult::SUCCESS)
			return PResult::GENERIC_ERROR;

		return PResult::SUCCESS;
	}

	std::optional<Socket> Socket::Accept()
	{
		sockaddr_in add = {};
		int len = sizeof(sockaddr_in);
		if (SocketHandle acceptSocketHandle = accept(m_handle, (sockaddr*)&add, &len);
			acceptSocketHandle != INVALID_SOCKET)
		{
			acceptedConnection = IPEndPoint((sockaddr*)&add);
			return Socket(IPVersion::IPv4, acceptSocketHandle);
		}
		m_lastError = GetLastError();
		return std::nullopt;
	}
	Socket::SocketHandle Socket::GetHandle() const
	{
		return m_handle;
	}
	IPVersion Socket::GetIPVersion() const
	{
		return m_version;
	}
	size_t Socket::GetSocketError() const
	{
		return m_lastError;
	}

	//---------------------------------------------------------
	PResult Socket::SetBlocking(bool isblocking)
	{
		unsigned long nonBlocking = 1;
		unsigned long blocking = 0;
		int result = ioctlsocket(m_handle, FIONBIO, isblocking ? &blocking : &nonBlocking);
		if (result == SOCKET_ERROR)
		{
			m_lastError = GetLastError();
			return PResult::GENERIC_ERROR;
		}
		return PResult::SUCCESS;
	}

	//-----------------------------------------------------------------------------
	IPEndPoint Socket::GetAcceptedEndPoint() const
	{
		return acceptedConnection;
	}

	//---------------------------------------------------------------------------------------
	IPEndPoint::IPEndPoint(const char* ip, unsigned short port)
		:m_ip_string(ip), m_port(port)
	{
		in_addr address;
		if (inet_pton(AF_INET, ip, &address) && address.S_un.S_addr != INADDR_NONE)
		{
			m_ip_string = m_ip_string;
			m_hostname = m_ip_string;
			m_bytes.resize(sizeof(ULONG));
			memcpy(&m_bytes[0], &address.S_un.S_addr, sizeof(ULONG));
		}
		else
		{
			addrinfo hints{};
			hints.ai_family = AF_INET;
			addrinfo* hostinfo = nullptr;
			if (!getaddrinfo(m_ip_string.c_str(), NULL, &hints, &hostinfo))
			{
				sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);
				m_hostname = m_ip_string;
				m_ip_string.resize(16);
				inet_ntop(AF_INET,&host_addr->sin_addr, &m_ip_string[0],16);
				ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
				m_bytes.resize(sizeof(ULONG));
				memcpy(&m_bytes[0], &ip_long, sizeof(ULONG));
				freeaddrinfo(hostinfo);
			}
		}
	}

	IPEndPoint::IPEndPoint(sockaddr* addr)
	{
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		m_version = IPVersion::IPv4;
		m_port = ntohs(addrv4->sin_port);
		m_bytes.resize(sizeof(ULONG));
		memcpy(&m_bytes[0], &addrv4->sin_addr, sizeof(ULONG));
		m_ip_string.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &m_ip_string[0], 16);
		m_hostname = m_ip_string;
	}
	sockaddr_in IPEndPoint::GetSocketAddressIPv4() const
	{
		//assert ipversion 4
		sockaddr_in addr {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr,&m_bytes[0],sizeof(ULONG));
		addr.sin_port = m_port;//htons
		return addr;
	}
}

