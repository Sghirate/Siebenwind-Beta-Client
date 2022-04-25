#pragma once

#include "Core/Minimal.h"
#include <vector>

namespace Core
{

struct PacketMessage
{
    PacketMessage(bool a_bigEndian = true);
    PacketMessage(u8* a_data, int a_dataSizeconst, bool a_bigEndian = true);
    PacketMessage(const std::vector<u8>& a_dataconst, bool a_bigEndian = true);
    virtual ~PacketMessage();

    void Append(u8* a_data, int a_dataSize);
    void Append(const std::vector<u8>& a_data);

    void Clear() { m_data.clear(); }

    std::vector<u8> Read(struct PacketReader* a_reader, int& a_dataOffset);

protected:
    std::vector<u8> m_data;

private:
    bool m_bigEndian = false;
};

}; // namespace Core
