// MIT License

namespace Wisp
{
CPacketMessage::CPacketMessage(bool bigEndian)
    : BigEndian(bigEndian)
{
}

CPacketMessage::CPacketMessage(u8 *data, int dataSize, bool bigEndian)
    : BigEndian(bigEndian)
{
    m_Data.resize(dataSize);
    memcpy(&m_Data[0], &data[0], dataSize);
}

CPacketMessage::CPacketMessage(const std::vector<u8> &data, bool bigEndian)
    : BigEndian(bigEndian)
    , m_Data(data)
{
}

CPacketMessage::~CPacketMessage()
{
    m_Data.clear();
}

void CPacketMessage::Append(u8 *data, int dataSize)
{
    std::vector<u8> buf(dataSize);
    memcpy(&buf[0], &data[0], dataSize);

    m_Data.insert(m_Data.end(), buf.begin(), buf.end());
}

void CPacketMessage::Append(const std::vector<u8> &data)
{
    m_Data.insert(m_Data.end(), data.begin(), data.end());
}

std::vector<u8> CPacketMessage::Read(class CPacketReader *reader, int &dataOffset)
{
    std::vector<u8> result;

    if (m_Data.empty())
    {
        return result;
    }

    int offsetToSize = 0;
    int wantSize = reader->GetPacketSize(m_Data, offsetToSize);

    if (wantSize == PACKET_VARIABLE_SIZE)
    {
        if (m_Data.size() < 3)
        {
            return result;
        }

        u8 *data = &m_Data[1];

        if (BigEndian)
        {
            wantSize = (data[0] << 8) | data[1];
        }
        else
        {
            wantSize = (data[1] << 8) | data[0];
        }

        dataOffset = offsetToSize + 2;
    }
    else
    {
        dataOffset = offsetToSize;
    }

    if ((int)m_Data.size() < wantSize)
    {
        return result;
    }

    result.insert(result.begin(), m_Data.begin(), m_Data.begin() + wantSize);

    m_Data.erase(m_Data.begin(), m_Data.begin() + wantSize);

    return result;
}

}; // namespace Wisp
