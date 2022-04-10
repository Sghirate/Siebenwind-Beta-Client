// MIT License

#ifndef WISP_PCKT_H
#define WISP_PCKT_H

namespace Wisp
{
class CPacketMessage
{
    bool BigEndian = false;

protected:
    std::vector<u8> m_Data;

public:
    CPacketMessage(bool bigEndian = true);
    CPacketMessage(u8 *data, int dataSizeconst, bool bigEndian = true);
    CPacketMessage(const std::vector<u8> &dataconst, bool bigEndian = true);
    virtual ~CPacketMessage();

    void Append(u8 *data, int dataSize);
    void Append(const std::vector<u8> &data);

    void Clear() { m_Data.clear(); }

    std::vector<u8> Read(class CPacketReader *reader, int &dataOffset);
};

}; // namespace Wisp

#endif // WISP_PCKT_H
