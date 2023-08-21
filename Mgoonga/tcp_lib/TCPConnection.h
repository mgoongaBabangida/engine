#pragma once

#include "Socket.h"
#include "Packet.h"

#include <base/Event.h>

namespace dbb
{
	//-------------------------------------------------------------
	class EXPORT TCPConnection
	{
	public:
		TCPConnection(Socket socket = {}, IPEndPoint endpoint = {});

		TCPConnection(const TCPConnection&);
		const TCPConnection& operator=(const TCPConnection&);

		TCPConnection(TCPConnection&&);
		void operator=(TCPConnection&&);

		void														Close();
		std::string											ToString();
		Socket													GetSocket();

		mutable Event<std::function<void(const std::string& _content, const std::string& _endpoint)>>						StringMessageRecieved;
		mutable Event<std::function<void(const std::vector<uint32_t>& _content, const std::string& _endpoint)>>	IntArrayMessageRecieved;

		std::pair<size_t, std::string>	Receive();

		bool														Send();
		void														AddSent(std::string&&);
		void														AddSent(std::vector<uint32_t>&&);

		void														ClearSentPacket();

		bool IsCurrentPacketSent() const;
		bool IsCurrentPacketReceived() const;

	protected:
		Socket			m_socket;
		IPEndPoint	endpoint;
		std::string stringRepresentation = "";

		std::optional<Packet> m_sent_pack;
		Packet								m_recv_pack;

		int				m_extraction_offset = 0;
		uint16_t	m_packet_size = 0;
		char			m_buffer[dbb::g_max_packetSize];
	};
}
