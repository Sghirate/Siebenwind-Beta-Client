#pragma once

#include "Core/DataStream.h"
#include <deque>
#include <vector>

namespace Wisp
{
class CPacketReader : public Core::StreamReader
{
    int MaxPacketStackSize = 5;

public:
    CPacketReader();
    virtual ~CPacketReader();

    void Read(class CConnection *connection);

    virtual int GetPacketSize(const std::vector<u8> &packet, int &offsetToSize) { return 0; }

protected:
    virtual void OnPacket() {}

    virtual void OnReadFailed() {}

public:
    std::deque<std::vector<u8>> m_PacketsStack;
};

}; // namespace Wisp
