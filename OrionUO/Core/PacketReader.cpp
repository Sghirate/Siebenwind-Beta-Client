#include "PacketReader.h"
#include "Core/Connection.h"
#include "Core/PacketMessage.h"

namespace Core
{

PacketReader::PacketReader()
{
}

PacketReader::~PacketReader()
{
}

void PacketReader::Read(Connection* connection)
{
    if (connection->ReadyRead())
    {
        if (!connection->Read())
        {
            OnReadFailed();
            return;
        }

        Core::PacketMessage* parser = connection->GetMessageParser();
        while (true)
        {
            int offset             = 0;
            std::vector<u8> packet = parser->Read(this, offset);

            if (packet.empty())
            {
                break;
            }

            if (kMaxPacketStackSize != 0)
            {
                m_PacketsStack.push_back(packet);
                if ((int)m_PacketsStack.size() > kMaxPacketStackSize)
                {
                    m_PacketsStack.pop_front();
                }
            }

            SetData((u8*)&packet[0], packet.size(), offset);
            OnPacket();
        }
    }
}

} // namespace Core
