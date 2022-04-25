#pragma once

#include "Core/DataStream.h"
#include "Core/Minimal.h"
#include <deque>
#include <vector>

namespace Core
{

struct PacketReader : public Core::StreamReader
{
    enum
    {
        kMaxPacketStackSize = 5
    };

    PacketReader();
    virtual ~PacketReader();

    void Read(struct Connection* connection);

    virtual int GetPacketSize(const std::vector<u8>& packet, int& offsetToSize) { return 0; }

protected:
    virtual void OnPacket() {}
    virtual void OnReadFailed() {}

private:
    std::deque<std::vector<u8>> m_PacketsStack;
};

} // namespace Core
