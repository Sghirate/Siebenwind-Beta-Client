#include "PacketMessage.h"
#include "Core/PacketReader.h"

namespace Core
{
PacketMessage::PacketMessage(bool a_bigEndian)
    : m_bigEndian(a_bigEndian)
{
}

PacketMessage::PacketMessage(u8* a_data, int a_dataSize, bool a_bigEndian)
    : m_bigEndian(a_bigEndian)
{
    m_data.resize(a_dataSize);
    memcpy(&m_data[0], &a_data[0], a_dataSize);
}

PacketMessage::PacketMessage(const std::vector<u8>& a_data, bool a_bigEndian)
    : m_bigEndian(a_bigEndian)
    , m_data(a_data)
{
}

PacketMessage::~PacketMessage()
{
    m_data.clear();
}

void PacketMessage::Append(u8* a_data, int a_dataSize)
{
    std::vector<u8> buf(a_dataSize);
    memcpy(&buf[0], &a_data[0], a_dataSize);

    m_data.insert(m_data.end(), buf.begin(), buf.end());
}

void PacketMessage::Append(const std::vector<u8>& a_data)
{
    m_data.insert(m_data.end(), a_data.begin(), a_data.end());
}

std::vector<u8> PacketMessage::Read(PacketReader* a_reader, int& a_dataOffset)
{
    std::vector<u8> result;

    if (m_data.empty())
    {
        return result;
    }

    int offsetToSize = 0;
    int wantSize = a_reader->GetPacketSize(m_data, offsetToSize);

    if (wantSize == 0) // TODO: was : const int PACKET_VARIABLE_SIZE = 0;
    {
        if (m_data.size() < 3)
        {
            return result;
        }

        u8* a_data = &m_data[1];

        if (m_bigEndian)
        {
            wantSize = (a_data[0] << 8) | a_data[1];
        }
        else
        {
            wantSize = (a_data[1] << 8) | a_data[0];
        }

        a_dataOffset = offsetToSize + 2;
    }
    else
    {
        a_dataOffset = offsetToSize;
    }

    if ((int)m_data.size() < wantSize)
    {
        return result;
    }

    result.insert(result.begin(), m_data.begin(), m_data.begin() + wantSize);

    m_data.erase(m_data.begin(), m_data.begin() + wantSize);

    return result;
}

}; // namespace Core
