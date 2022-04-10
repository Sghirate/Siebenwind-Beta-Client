
#ifndef WISP_CON_H
#define WISP_CON_H

#include "../Sockets.h"
namespace Wisp
{
class CConnection
{
public:
    tcp_socket m_Socket = nullptr;

    int DataReady = 0;
    int Port = 0;

    bool Connected = 0;

    CConnection();
    virtual ~CConnection();
    void Init();

    class CPacketMessage *m_MessageParser;

    virtual bool Connect(const std::string &address, u16 port);
    void Disconnect();

    bool ReadyRead();

    virtual std::vector<u8> Decompression(vector<u8> data) { return data; }

    bool Read(int maxSize = 0x1000);

    int Send(u8 *data, int size);
    int Send(const std::vector<u8> &data);
};
}; // namespace Wisp

#endif // WISP_CON_H
