#include "pch.h"
#include "Packet.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>

#include <base/Log.h>

namespace dbb
{
	Packet::Packet(PacketType type)
	{
		m_buffer.resize(sizeof(PacketType));
		AssignPacketType(type);
	}

	PacketType Packet::GetPacketType()
	{
		PacketType* t = reinterpret_cast<PacketType*>(&m_buffer[0]);
		return static_cast<PacketType>(*t);  //htons;
	}

	void Packet::AssignPacketType(PacketType type)
	{
		PacketType* t = reinterpret_cast<PacketType*>(&m_buffer[0]);
		*t = static_cast<PacketType>(type);  //htons
		m_extraction_offset = sizeof(PacketType);
	}

	void Packet::Clear()
	{
		m_buffer.resize(sizeof(dbb::PacketType));
		AssignPacketType(dbb::PacketType::PT_Invalid);
		task = dbb::Packet::PacketTask::ProcessPacketSize;
	}

	void Packet::Append(void* data, int size)
	{
		m_buffer.insert(m_buffer.end(), (char*)data, (char*)data + size);
	}

	Packet& Packet::operator<<(uint32_t _data)
	{
		auto data = _data;//htonl
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	Packet& Packet::operator>>(uint32_t& data)
	{
		data = *reinterpret_cast<uint32_t*>(&m_buffer[m_extraction_offset]);
		data = data;//ntohl?
		m_extraction_offset += sizeof(uint32_t);
		return *this;
	}

	Packet& Packet::operator<<(const std::string& data)
	{
		*this << (uint32_t)data.size();
		Append((void*)data.data(), data.size());
		return *this;
	}

	Packet& Packet::operator>>(std::string& data)
	{
		data.clear();
		uint32_t stringSize = 0;
		*this >> stringSize;
		data.resize(stringSize);
		data.assign(&m_buffer[m_extraction_offset], stringSize);
		m_extraction_offset += stringSize;
		return *this;
	}
}

//-------------------------------------------------------------------
std::pair<bool, dbb::PacketContent> ProcessPacket(dbb::Packet& packet) // should return processed results void*
{
	std::pair<bool, dbb::PacketContent> result{ true, {} };
	switch (packet.GetPacketType())
	{
	case dbb::PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		packet >> chatmessage;
		result.second = chatmessage;
		base::Log("Chat Message: " + chatmessage);
		break;
	}
	case dbb::PacketType::PT_Intarray:
	{
		uint32_t arraySize = 0;
		std::vector<uint32_t> res;
		packet >> arraySize;
		base::Log("Array Size: " + arraySize);
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t element = 0;
			packet >> element;
			res.push_back(element);
			base::Log("Element[" + std::to_string(i) + "] - " + std::to_string(element));
		}
		result.second = res;
		break;
	}
	default:
		base::Log("Unrecognized packet type: " + (int)packet.GetPacketType());
		result.first = true;
	}

	return result;
}

