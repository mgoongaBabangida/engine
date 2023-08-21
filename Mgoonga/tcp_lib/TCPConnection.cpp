#include "pch.h"
#include "TCPConnection.h"

namespace dbb
{
	TCPConnection::TCPConnection(Socket socket, IPEndPoint endpoint)
		:m_socket(socket), endpoint(endpoint)
	{
		stringRepresentation = "[" + endpoint.m_hostname;
		stringRepresentation += ":" + endpoint.m_ip_string + "]";
	}
	Socket TCPConnection::GetSocket()
	{ 
		return m_socket;
	}

	TCPConnection::TCPConnection(const TCPConnection& _other)
	{
		*this = _other;
	}

	const TCPConnection& TCPConnection::operator=(const TCPConnection& _other)
	{
		m_socket = _other.m_socket;
		endpoint = _other.endpoint;
		stringRepresentation = _other.stringRepresentation;

		StringMessageRecieved = _other.StringMessageRecieved;
		IntArrayMessageRecieved = _other.IntArrayMessageRecieved;

		m_sent_pack = _other.m_sent_pack;
		m_recv_pack = _other.m_recv_pack;

		m_extraction_offset = _other.m_extraction_offset;
		m_packet_size = _other.m_packet_size;
		m_buffer[dbb::g_max_packetSize] = _other.m_buffer[dbb::g_max_packetSize];
		return *this;
	}

	TCPConnection::TCPConnection(TCPConnection&& _other)
	{
		*this = std::move(_other);
	}

	void TCPConnection::operator=(TCPConnection&& _other)
	{
		m_socket = std::move(_other.m_socket);
		endpoint = std::move(_other.endpoint);
		stringRepresentation = std::move(_other.stringRepresentation);

		StringMessageRecieved = std::move(_other.StringMessageRecieved);
		IntArrayMessageRecieved = std::move(_other.IntArrayMessageRecieved);

		m_sent_pack = std::move(_other.m_sent_pack);
		m_recv_pack = std::move(_other.m_recv_pack);

		m_extraction_offset = _other.m_extraction_offset;
		m_packet_size = _other.m_packet_size;
		m_buffer[dbb::g_max_packetSize] = std::move(_other.m_buffer[dbb::g_max_packetSize]);
	}

	void TCPConnection::Close()
	{
		m_socket.Close();
	}

	std::string TCPConnection::ToString()
	{
		return stringRepresentation;
	}

	std::pair<size_t, std::string> TCPConnection::Receive()
	{
		int bytesReceived = 0;
		std::string error;
		if (m_recv_pack.task == dbb::Packet::PacketTask::ProcessPacketSize)
		{
			bytesReceived = recv(m_socket.GetHandle(), (char*)&m_packet_size + m_extraction_offset, sizeof(uint16_t) - m_extraction_offset, 0);
			if (bytesReceived == 0)
				error = "Recv==0";
			else if (bytesReceived == SOCKET_ERROR)
			{
				if (int i_error = WSAGetLastError(); i_error != WSAEWOULDBLOCK)
					error = "Recv<0";
			}
			else if (bytesReceived > 0)
			{
				m_extraction_offset += bytesReceived;
				if (m_extraction_offset == sizeof(uint16_t))
				{
					m_packet_size = m_packet_size; //htons
					if (m_packet_size > dbb::g_max_packetSize)
						error = "Packet size too large.";
					m_extraction_offset = 0;
					m_recv_pack.task = dbb::Packet::PacketTask::ProcessPacketContents;
				}
			}
		}
		else if (m_recv_pack.task == dbb::Packet::PacketTask::ProcessPacketContents)
		{
			bytesReceived = recv(m_socket.GetHandle(), (char*)&m_buffer + m_extraction_offset, m_packet_size - m_extraction_offset, 0);
			if (bytesReceived == 0)
				error = "Recv==0";
			else if (bytesReceived == SOCKET_ERROR)
			{
				if (int i_error = WSAGetLastError(); i_error != WSAEWOULDBLOCK)
					error = "Recv<0";
			}
			else if (bytesReceived > 0)
			{
				m_extraction_offset += bytesReceived;
				if (m_extraction_offset == m_packet_size)
				{
					m_recv_pack.m_buffer.resize(m_packet_size);
					memcpy(&m_recv_pack.m_buffer[0], m_buffer, m_packet_size);
					if (auto res = ProcessPacket(m_recv_pack); !res.first)
						error = "Failed to process packet";
					else
					{
						if (const std::string* pcontent = std::get_if<std::string>(&res.second))
						{
							StringMessageRecieved.Occur(*pcontent, stringRepresentation);
						}
						else if (const std::vector<uint32_t > * pcontent = std::get_if<std::vector<uint32_t>>(&res.second))
						{
							IntArrayMessageRecieved.Occur(*pcontent, stringRepresentation);
						}
						m_recv_pack.Clear();
					}
					m_packet_size = 0;
					m_extraction_offset = 0;
				}
			}
		}
		return { bytesReceived , error };
	}

	void TCPConnection::AddSent(std::string&& sent)
	{
		dbb::Packet msg(dbb::PacketType::PT_ChatMessage);
		msg << sent;
		m_sent_pack = msg;
	}

	void TCPConnection::AddSent(std::vector<uint32_t>&& sent)
	{
		dbb::Packet msg(dbb::PacketType::PT_Intarray);
		for(uint32_t i : sent)
			msg << i ;
		m_sent_pack = msg;
	}

	bool TCPConnection::Send()
	{
		if (m_sent_pack.has_value())
		{
			if (m_sent_pack->task == dbb::Packet::PacketTask::ProcessPacketSize)
			{
				uint16_t bigEndianPacketSize = m_sent_pack->m_buffer.size(); //htons
				int bytesSent = send(m_socket.GetHandle(), (char*)(&bigEndianPacketSize), sizeof(uint16_t), NULL);
				if (bytesSent > 0)
				{
					m_sent_pack->m_extraction_offset = 0;
					m_sent_pack->task = dbb::Packet::PacketTask::ProcessPacketContents;
				}
				else
					return false;
			}
			else //Sending packet contents
			{
				char* bufferPtr = m_sent_pack->m_buffer.data();
				int bytesSent = send(m_socket.GetHandle(), (char*)(bufferPtr)+m_sent_pack->m_extraction_offset, m_sent_pack->m_buffer.size() - m_sent_pack->m_extraction_offset, 0);
				if (bytesSent > 0)
					m_sent_pack->m_extraction_offset += bytesSent;
				else
					return false;
			}
		}
		return false;
	}

	void TCPConnection::ClearSentPacket()
	{
		m_sent_pack = std::nullopt;
	}

	bool TCPConnection::IsCurrentPacketSent() const
	{
		if (m_sent_pack.has_value())
			return m_sent_pack->m_extraction_offset == m_sent_pack->m_buffer.size();
		else
			return true;
	}

	bool TCPConnection::IsCurrentPacketReceived() const
	{
		return m_extraction_offset == m_packet_size;
	}
}

