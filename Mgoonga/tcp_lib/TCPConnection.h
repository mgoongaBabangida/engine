#pragma once

#include "Socket.h"
#include "Packet.h"

namespace dbb
{
	class EXPORT TCPConnection
	{
	public:
		TCPConnection(Socket socket = {}, IPEndPoint endpoint = {});
		void Close();
		std::string ToString();
		Socket GetSocket();
		std::pair<size_t, std::string> Receive();
		void AddSent(std::string&&);
		bool Send();
		void ClearSentPacket();

		bool IsCurrentPacketSent() const;
		bool IsCurrentPacketReceived() const;

	protected:
		Socket m_socket;
		IPEndPoint endpoint;
		std::string stringRepresentation = "";

		std::optional<Packet> m_sent_pack;
		Packet m_recv_pack;

		int m_extraction_offset = 0;
		uint16_t m_packet_size = 0;
		char m_buffer[dbb::g_max_packetSize];
	};
}
