#pragma once
#include "pch.h"
#include <vector>
#include <string>

namespace dbb
{
	enum class PacketType
	{
		PT_Invalid,
		PT_ChatMessage,
		PT_Intarray
	};

	class EXPORT Packet
	{
		friend class Socket;
	public:
		enum PacketTask
		{
			ProcessPacketSize,
			ProcessPacketContents,
			ProcessDone
		};

		Packet(PacketType = PacketType::PT_Invalid);

		PacketType GetPacketType();
		void AssignPacketType(PacketType);

		void Clear();
		void Append(void* data, int size);

		Packet& operator <<(uint32_t data);
		Packet& operator >>(uint32_t& data);

		Packet& operator <<(const std::string& data);
		Packet& operator >>(std::string& data);
	/*protected:*/
		std::vector<char> m_buffer;
		uint32_t m_extraction_offset = 0;
		PacketTask task = PacketTask::ProcessPacketSize;
	};
}

bool EXPORT ProcessPacket(dbb::Packet& packet);


