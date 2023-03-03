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
					if (!ProcessPacket(m_recv_pack))
						error = "Failed to process packet";
					else
						m_recv_pack.Clear();
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

